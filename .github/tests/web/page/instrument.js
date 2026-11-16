// Counts frames and draws and records the WebGL context, from outside the app.
(function instrument() {
  const h = (window.__harness = {
    contextType: null,
    glRenderer: null,
    glVersion: null,
    frames: 0,
    drawCalls: 0,
    contextLost: false,
    glError: false,
  });
  // Context-lost normally pops an alert() which would block headless; capture instead.
  window.alert = () => { h.contextLost = true; };

  const origGetContext = HTMLCanvasElement.prototype.getContext;
  HTMLCanvasElement.prototype.getContext = function (type, ...rest) {
    const ctx = origGetContext.call(this, type, ...rest);
    if (ctx && /webgl/i.test(String(type))) {
      h.contextType = String(type);
      try {
        const dbg = ctx.getExtension('WEBGL_debug_renderer_info');
        h.glRenderer = dbg
          ? ctx.getParameter(dbg.UNMASKED_RENDERER_WEBGL)
          : ctx.getParameter(ctx.RENDERER);
        h.glVersion = ctx.getParameter(ctx.VERSION);
      } catch { /* ignore */ }
      this.addEventListener('webglcontextlost', () => { h.contextLost = true; }, false);
      for (const m of ['drawArrays', 'drawElements', 'drawArraysInstanced', 'drawElementsInstanced']) {
        const orig = ctx[m];
        if (typeof orig === 'function') {
          ctx[m] = function (...a) { h.drawCalls++; return orig.apply(this, a); };
        }
      }
    }
    return ctx;
  };

  const origRAF = window.requestAnimationFrame;
  window.requestAnimationFrame = function (cb) {
    return origRAF.call(window, (t) => { h.frames++; return cb(t); });
  };

  // Surface uncaught exceptions with their full stack. emscripten's generated
  // HTML installs its own window.onerror that only logs a one-line message, and
  // a C++ exception reaching JS often arrives as a bare value with no JS stack.
  // addEventListener coexists with that onerror; route the richest detail we can
  // get (Error.stack, or the emscripten exception message) through console.error
  // so it lands in the captured output.log. getExceptionMessage is exported by
  // emscripten when built with exception support.
  const describe = (e) => {
    if (!e) return '<no error object>';
    try {
      const M = window.Module;
      if (typeof e === 'number' && M && typeof M.getExceptionMessage === 'function') {
        return 'C++ exception: ' + M.getExceptionMessage(e).join(': ');
      }
    } catch (inner) { /* fall through to generic */ }
    return e.stack || e.message || String(e);
  };
  window.addEventListener('error', (ev) => {
    console.error('[uncaught error] ' + describe(ev.error !== undefined ? ev.error : ev.message));
  });
  window.addEventListener('unhandledrejection', (ev) => {
    console.error('[unhandledrejection] ' + describe(ev.reason));
  });
})();
