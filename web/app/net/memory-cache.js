(function(){
  'use strict';

  var storage = {};
  var dirty   = true;

  function put(key, data, date) {
    storage[key] = {data:data, date:date, dirty:false};
  }

  function get(key) {
    if (!storage.hasOwnProperty(key))
      return null;

    return storage[key];
  }

  function contains(key) {
    if (!storage.hasOwnProperty(key))
      return 0;

    return storage[key].date;
  }

  function open() {
    dirty = false;
  }

  function close() {
    if (dirty)
      return;

    for (var key in storage) if (storage.hasOwnProperty(key)) {
      storage[key].dirty = true;
    }

    dirty = true;
  }

  if (typeof window !== 'undefined') {
    schat.net.cache = {
      put: put,
      get: get,
      contains: contains,
      close: close,
      open: open
    };
  } else {
    exports.put      = put;
    exports.get      = get;
    exports.contains = contains;
    exports.close    = close;
    exports.open     = open;
  }
})();
