/* Linked with --pre-js. Snapshots the keys of the Emscripten Fetch cache
 * (IndexedDB emscripten_filesystem -> FILES) into Module.blamOfflineFiles
 * before main() runs, so offline_maps.cpp can list uploaded maps
 * synchronously. Maps are put there by BlamMapUpload.
 *
 * The database is opened at version 1 like Fetch.init() does: a higher
 * version would make Emscripten's own open fail, and Fetch clears the store
 * on any upgrade. */
Module.preRun = Module.preRun || [];
Module.preRun.push(function() {
  Module.blamOfflineFiles = [];
  if (typeof indexedDB === 'undefined')
    return;

  addRunDependency('blam_offline_maps');
  var finished = false;
  var done = function() {
    if (finished)
      return;
    finished = true;
    removeRunDependency('blam_offline_maps');
  };
  try {
    var open = indexedDB.open('emscripten_filesystem', 1);
    open.onupgradeneeded = function(event) {
      var db = event.target.result;
      if (!db.objectStoreNames.contains('FILES'))
        db.createObjectStore('FILES');
    };
    open.onsuccess = function(event) {
      var db = event.target.result;
      try {
        var keys = db.transaction(['FILES'], 'readonly')
                     .objectStore('FILES')
                     .getAllKeys();
        keys.onsuccess = function() {
          Module.blamOfflineFiles = keys.result.filter(function(key) {
            return typeof key === 'string';
          });
          console.log('[blam] ' + Module.blamOfflineFiles.length +
                      ' file(s) in offline map storage');
          db.close();
          done();
        };
        keys.onerror = function() { db.close(); done(); };
      } catch (e) {
        console.warn('[blam] offline map storage unreadable: ' + e);
        db.close();
        done();
      }
    };
    open.onerror = done;
  } catch (e) {
    console.warn('[blam] IndexedDB unavailable: ' + e);
    done();
  }
});
