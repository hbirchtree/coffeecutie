// Records the first GL errors after each call, read back with __glTraceDump().
(function installGLTracer() {
  const WATCH = ['drawElements','drawElementsInstanced','drawArrays','drawArraysInstanced',
    'deleteBuffer','deleteVertexArray','bindBuffer','bindBufferRange','bindBufferBase','bufferData','bufferSubData','texStorage2D','texImage2D',
    'texSubImage2D','compressedTexImage2D','compressedTexSubImage2D','texParameteri',
    'framebufferTexture2D','bindFramebuffer','blitFramebuffer','readPixels','useProgram',
    'bindVertexArray','vertexAttribPointer','vertexAttribIPointer','uniformBlockBinding',
    'linkProgram','bindAttribLocation','drawBuffers','invalidateFramebuffer','renderbufferStorage',
    'uniformMatrix4fv','uniformMatrix3fv','uniform1i','uniform1f','uniform2fv','uniform3fv',
    'uniform4fv','uniform1iv','uniform2f','uniform1ui','activeTexture','bindTexture','bindSampler',
    'samplerParameteri','samplerParameterf','texParameterf','generateMipmap','checkFramebufferStatus',
    'framebufferRenderbuffer','clear','clearColor','viewport','enableVertexAttribArray'];
  const counts = Object.create(null);
  let total = 0;
  let frameNo = 0;
  let ebZero = 0;
  const ERR = {1280:'INVALID_ENUM',1281:'INVALID_VALUE',1282:'INVALID_OPERATION',
               1285:'OUT_OF_MEMORY',1286:'INVALID_FRAMEBUFFER_OPERATION',37442:'CONTEXT_LOST'};
  function wrap(proto) {
    if (!proto || proto.__glTraced) return;
    proto.__glTraced = true;
    for (const name of WATCH) {
      const orig = proto[name];
      if (typeof orig !== 'function') continue;
      proto[name] = function (...args) {
        if (name === 'drawArrays' || name === 'drawElements' ||
            name === 'drawArraysInstanced' || name === 'drawElementsInstanced') {
          const k = `CALL ${name}(mode=${args[0]},a=${args[1]},b=${args[2]})`;
          counts[k] = (counts[k] || 0) + 1;
          const f = `FN ${name} mode=${args[0]}`;
          counts[f] = (counts[f] || 0) + 1;
          if (name === 'drawArrays' && args[0] === 6 && args[2] === 4) {
            frameNo++;
            if (frameNo % 10 === 0) {
              const rows = Object.entries(counts).filter(([k]) => k.startsWith('FN '))
                .sort((a, b) => b[1] - a[1]).slice(0, 8);
              console.log(`[GLFRAME ${frameNo}] ` + rows.map(([k, v]) => `${k}=${v}`).join('  '));
            }
          }
        }
        if (name === 'deleteBuffer' && ebZero < 6) {
          ebZero++;
          console.log('[DELBUF] deleteBuffer\n' + new Error().stack);
        }
        if (name === 'bindBuffer' && args[0] === 34963 && !args[1] && ebZero < 3) {
          ebZero++;
          console.log('[EBZERO] unbind ELEMENT_ARRAY_BUFFER\n' + new Error().stack);
        }
        const r = orig.apply(this, args);
        if (total < 4000) {
          const e = this.getError();
          if (e !== 0) {
            total++;
            let a = '';
            try {
              a = args.map((v) => (typeof v === 'number' ? v
                    : v && v.constructor ? v.constructor.name : String(v))).join(',');
            } catch { a = '?'; }
            const key = `${name}(${a}) -> ${ERR[e] || e}`;
            counts[key] = (counts[key] || 0) + 1;
            if (counts[key] <= 3) console.log('[GLTRACE] ' + key);
          }
        }
        return r;
      };
    }
  }
  wrap(window.WebGL2RenderingContext && window.WebGL2RenderingContext.prototype);
  wrap(window.WebGLRenderingContext && window.WebGLRenderingContext.prototype);
  setInterval(() => {
    const rows = Object.entries(counts).filter(([k]) => k.startsWith('FN '))
      .sort((a, b) => b[1] - a[1]).slice(0, 12);
    if (rows.length) console.log('[GLCENSUS] ' + rows.map(([k, v]) => `${k}=${v}`).join('  '));
  }, 5000);
  window.__glTraceDump = function () {
    const rows = Object.entries(counts).sort((a, b) => b[1] - a[1]).slice(0, 60);
    return rows.map(([k, v]) => `${String(v).padStart(6)}  ${k}`).join('\n');
  };
})();
