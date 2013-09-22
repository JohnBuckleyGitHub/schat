(function() {
  'use strict';

  var storage = {};
  var cache   = {};

  cache.put = function(key, data, date, persistent) {
    persistent   = persistent || true;
    var value    = [ date, data ];
    storage[key] = value;

    if (persistent)
      localStorage.setItem('cache.' + key, JSON.stringify(value));
  };

  cache.get = function(key, persistent) {
    persistent = persistent || true;
    var value  = storage.hasOwnProperty(key) ? storage[key] : null;

    if (value === null && persistent) {
      try {
        value = JSON.parse(localStorage.getItem('cache.' + key));
      }
      catch (e) {}
    }

    if (value === null)
      return value;

    storage[key] = value;
    return value[1];
  };

  cache.date = function(key, persistent) {
    if (storage.hasOwnProperty(key))
      return storage[key][0];

    if (cache.get(key, persistent) === null)
      return 0;

    return storage[key][0];
  };

  window.schat.net.storage = cache;
})();