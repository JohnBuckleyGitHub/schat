var packets = require('./packets');
var cache   = require('./memory-cache');
var ChatId  = require('../id/cid').ChatId;
var users   = require('./users');
var core    = require('../core.js');

var proxy = {};
var bind = {};

var Subscribers = {
  _open: false
};

/**
 * Автоматический запрос списка подписчиков при необходимости.
 *
 * @param key
 */
Subscribers.get = function(key) {
  key += '/sub';
  var value = cache.get(key);

  if (value === null || value.dirty) {
    var packet = {request:key, id:ChatId.messageId()};

    bind[packet.id] = [packet, null];
    core.parent.sendJSON(JSON.stringify(packets.write(packet, 'REQ')));
  }
};


/**
 * Проверка подписки пользователя.
 *
 * @param path
 * @param id
 * @returns {boolean} true в случае если пользователь подписан, в этом случае будет использован локальный кэш,
 * в противном случае запрос будет перенаправлен на вышестоящий сервер.
 */
Subscribers.check = function(path, id) {
  var channel = path.split('/', 1)[0];
  path = path.substring(channel.length + 1);

  var sub = cache.get(channel + '/sub');
  if (sub === null || sub.dirty)
    return false;

  sub = sub.data[0];
  return (sub.hasOwnProperty(path) && sub[path].hasOwnProperty(id));
};


Subscribers.broadcast = function(data) {
  var channel = data.id.split('/', 1)[0];
  var sub     = cache.get(channel + '/sub');
  if (sub === null || sub.dirty)
    return;

  sub = sub.data[0];
  var channels = {};

  for (var path in sub) if (sub.hasOwnProperty(path)) {
    var listeners = sub[path];
    for (var id in listeners) if (listeners.hasOwnProperty(id)) {
      channels[id] = channels[id] || 0;
      channels[id]++;
    }
  }

  var conns = users.get(channels);
  if (!conns.length)
    return;

  var packet = packets.write(data, 'RES');
  for (var i = 0; i < conns.length; i++)
    conns[i].write(packet);
};


Subscribers.open = function() {
  Subscribers._open = true;

  var conns = users.all();
  if (!conns.length)
    return;

  var packet = packets.write({id:core.serverId() + '/parent', data:core.serverId()}, 'RES');
  for (var i = 0; i < conns.length; i++)
    conns[i].write(packet);
};


Subscribers.close = function() {
  if (!Subscribers._open)
    return;

  Subscribers._open = false;
  var conns = users.all();
  if (!conns.length)
    return;

  var packet = packets.write({id:core.serverId() + '/parent', data:null}, 'RES');
  for (var i = 0; i < conns.length; i++)
    conns[i].write(packet);
};


/**
 * Проверка кэша, в случае если ресурс найден в кэше, запрос на вышестоящий сервер произведён не будет.
 *
 * @param data
 * @param conn
 * @returns {boolean} true если запрос был обработан.
 */
function checkCache(data, conn) {
  var exists = cache.get(data.request);
  if (exists === null || exists.dirty)
    return false;

  if (!Subscribers.check(data.request, conn.channelId))
    return false;

  if (exists.date == data.date)
    conn.write(packets.write({status:304, id:data.id}, 'REP'));
  else
    conn.write(packets.write({data:exists.data, date:exists.date, id:data.id}, 'REP'));

  return true;
}


/**
 * Обработка запроса от локального клиента.
 *
 * @param data
 * @param conn
 */
function onRequest(data, conn) {
  if (data.method === 'AUTH' || bind.hasOwnProperty(data.id))
    return;

  if (data.method === 'GET') {
    if (data.request.indexOf('server') == 0)
      data.request = data.request.replace('server', core.serverId());

    if (checkCache(data, conn))
      return;
  }

  var date = data.date;
  if (date && !cache.contains(data.request))
    data.date = 0;

  bind[data.id] = [data, conn];

  data.headers.ip   = conn.xForwardedFor || conn.remoteAddress;
  data.headers.user = null;

  core.parent.sendJSON(JSON.stringify(packets.write(data)));
  data.date = date;
}


/**
 * Обработка ответа от вышестоящего сервера.
 *
 * @param reply
 */
function onReply(reply) {
  if (!bind.hasOwnProperty(reply.id))
    return;

  var req = bind[reply.id][0];

  if (reply.status === 200 && reply.date > 0 && req.method === 'GET') {
    cache.put(req.request, reply.data, reply.date);
    Subscribers.get(req.request.split('/', 1)[0]);

    if (req.date && req.date === reply.date) {
      bind[reply.id][1].write(packets.write({status:304, headers:reply.headers, id:reply.id}, 'REP'));
      delete bind[reply.id];
      return;
    }
  }

  if (bind[reply.id][1] !== null)
    bind[reply.id][1].write(packets.write(reply));

  delete bind[reply.id];
}


function onResource(data) {
  cache.put(data.id, data.data, data.date);

  Subscribers.broadcast(data);
}

exports.init = function() {
  core.parent._socket.on('connect', cache.open);
  core.parent._socket.on('auth-reply', Subscribers.open);
  core.parent._socket.on('close',   cache.close);
  core.parent._socket.on('close',   Subscribers.close);

  core.server.on('packet.REQ', onRequest);
  core.parent.on('packet.REP', onReply);
  core.parent.on('packet.RES', onResource);
};
