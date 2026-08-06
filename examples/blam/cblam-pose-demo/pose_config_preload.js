/* Linked with --pre-js so it runs inside the module scope, where FS and
 * addRunDependency() are in scope without being exported on Module (the
 * project only exports ccall,cwrap,HEAPU32,HEAPU8 — see
 * src/peripheral/CMakeLists.txt:218).
 *
 * pose_config.cpp reads /pose_config.json synchronously during app load,
 * before the map fetch starts, so the file has to exist before main() runs.
 * addRunDependency holds startup until the fetch settles.
 *
 * A missing or unreadable config is not fatal: nothing is written and
 * load_pose_config() falls back to its built-in defaults. */
Module.preRun = Module.preRun || [];
Module.preRun.push(function() {
  var url = 'configs/cyborg.json';
  try {
    url = new URLSearchParams(location.search).get('config') || url;
  } catch (e) { /* no location (worker/node): keep the default */ }

  addRunDependency('pose_config');
  fetch(url)
    .then(function(response) {
      if (!response.ok)
        throw new Error(response.status + ' ' + response.statusText);
      return response.text();
    })
    .then(function(text) {
      FS.writeFile('/pose_config.json', text);
      console.log('[pose_demo] config ' + url + ' staged at /pose_config.json');
    })
    .catch(function(e) {
      console.warn(
        '[pose_demo] config ' + url + ' unavailable (' + e.message +
        '), using built-in defaults');
    })
    .finally(function() {
      removeRunDependency('pose_config');
    });
});
