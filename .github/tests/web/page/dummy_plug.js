// Bridges injected dummy plug events into the engine's exported entry points.
(function installDummyPlugLive() {
  const mod = () => window.Module || window.BlamModule;
  window.__coffeeAppLoaded = function () {
    const M = mod();
    if (!M || typeof M.ccall !== 'function') return false;
    try {
      return M.ccall('coffee_app_loaded', 'number', [], []) === 1;
    } catch {
      return false;
    }
  };
  window.__coffeeDummyEvent = function (json) {
    const M = mod();
    if (!M || typeof M.ccall !== 'function') return false;
    try {
      M.ccall('coffee_dummy_plug_event', null, ['string'], [json]);
    } catch (e) {
      let msg = (e && (e.stack || e.message)) || String(e);
      if (typeof e === 'number' && typeof M.getExceptionMessage === 'function') {
        try { msg = 'C++ exception: ' + M.getExceptionMessage(e).join(': '); } catch { /* keep msg */ }
      }
      console.error('[dummy_plug inject] ' + msg);
      throw e;
    }
    return true;
  };
})();
