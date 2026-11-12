/* Linked with --pre-js. Snapshots the files in FileMapper's store (the OPFS
 * root, mounted as /maps) into Module.blamOfflineFiles before main() runs, so
 * offline_maps.cpp can list uploaded maps synchronously. Maps are put there
 * by BlamMapUpload, at OPFS pc/bloodgulch.map for /maps/pc/bloodgulch.map.
 * Files at the root are the store's own downloads, not uploads. */
Module.preRun = Module.preRun || [];
Module.preRun.push(function() {
  Module.blamOfflineFiles = [];
  if (typeof navigator === 'undefined' || !navigator.storage ||
      !navigator.storage.getDirectory)
    return;

  var walk = async function(dir, path) {
    for await (var [name, handle] of dir.entries()) {
      if (handle.kind === 'directory')
        await walk(handle, path + '/' + name);
      else if (path !== '/maps')
        Module.blamOfflineFiles.push(path + '/' + name);
    }
  };

  addRunDependency('blam_offline_maps');
  (async function() {
    await walk(await navigator.storage.getDirectory(), '/maps');
    console.log('[blam] ' + Module.blamOfflineFiles.length +
                ' file(s) in offline map storage');
  })().catch(function(e) {
    console.warn('[blam] offline map storage unreadable: ' + e);
  }).finally(function() {
    removeRunDependency('blam_offline_maps');
  });
});
