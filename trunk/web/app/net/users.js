var storage = {};

function size(obj) {
  var size = 0;
  for (var key in obj) if (obj.hasOwnProperty(key)) {
    ++size;
  }

  return size;
}


function add(conn) {
  conn.on('close', function() {
    delete storage[conn.channelId][conn.id];
    if (!size(storage[conn.channelId]))
      delete storage[conn.channelId];
  });

  storage[conn.channelId] = storage[conn.channelId] || {};
  storage[conn.channelId][conn.id] = conn;
}


function get(channels) {
  var out = [];
  for (var key in channels) if (channels.hasOwnProperty(key)) {
    if (!storage.hasOwnProperty(key))
      continue;

    var conns = storage[key];
    for (var id in conns) if (conns.hasOwnProperty(id)) {
      out.push(conns[id]);
    }
  }

  return out;
}


function all() {
  var out = {};
  for (var key in storage) if (storage.hasOwnProperty(key)) {
    var conns = storage[key];
    for (var id in conns) if (conns.hasOwnProperty(id)) {
      out[id] = conns[id];
    }
  }

  var array = [];
  for (id in out) if (out.hasOwnProperty(id)) {
    array.push(out[id]);
  }

  return array;
}


exports.add = add;
exports.all = all;
exports.get = get;
