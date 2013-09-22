var packet  = require('./packet');
var proto   = require('./protocol');
var cid     = require('../id/cid');
var packets = require('./packets');

var url     = require('url');
var http    = require('http');
var https   = require('https');

var Auth = {
  server: 'https://auth.schat.me',

  get: function(options, callback) {
    if (typeof options === 'string')
      options = url.parse(options);

    if (options.protocol == 'https:')
      return https.get(options, callback);

    return http.get(options, callback);
  }
};


/**
 * Запрос списка провайдеров.
 *
 * @param data
 * @param conn
 */
Auth.providers = function providers(data, conn) {
  var req = this.get(this.server + '/providers.json', function(res) {
    if (res.statusCode !== 200) {
      conn.write(packets.error(data.id, 502, res.statusCode));
      return;
    }

    var reply = '';

    res.on('data', function(chunk) { reply += chunk; });
    res.on('end', function() {
      try {
        var providers = JSON.parse(reply);
      }
      catch (e) {
        conn.write(packets.error(data.id, 502, e.message));
      }

      var secret = cid.ChatId.randomId(cid.PASSWORD_ID).toString();
      var state  = cid.ChatId.create(url.parse(this.server).hostname + secret, cid.MESSAGE_ID).toString() + cid.toBase32(data.data[0]);

      conn.write(packets.reply({method:'AUTH',id:data.id, data:[providers, state, secret]}));
    }.bind(this));
  }.bind(this));

  req.on('error', function(e) {
    conn.write(packets.error(data.id, 500, e.message));
  });
};


Auth.state = function state(data, conn) {
  var options = url.parse(this.server + '/state/' + data.data[0]);
  options.headers = { 'X-SChat-Secret': data.data[1] };

  var req = this.get(options, function(res) {
    if (res.statusCode !== 200) {
      conn.write(packets.error(data.id, 502, res.statusCode));
      return;
    }

    var reply = '';

    res.on('data', function(chunk) { reply += chunk; });
    res.on('end', function() {
      try {
        var state = JSON.parse(reply);
      }
      catch (e) {
        conn.write(packets.error(data.id, 502, e.message));
      }

      conn.write(packets.reply({method:'AUTH',id:data.id, data:[state, null]}));
    }.bind(this));
  }.bind(this));

  req.on('error', function(e) {
    conn.write(packets.error(data.id, 500, e.message));
  });
};


function request(cookie) {
  var header = packet.basic(proto.AUTH_REQUEST_PACKET);
  var offset = header.length;
  var buf    = new Buffer(offset + 84);
  header.copy(buf);

  buf.writeUInt8(0, offset++);
  buf.writeUInt8(99, offset++);

  var uniqueId = new cid.ChatId();
  uniqueId.setType(cid.UNIQUE_USER_ID);
  uniqueId._data.copy(buf, offset);
  offset += 21;

  cid.ChatId.randomId(cid.MESSAGE_ID)._data.copy(buf, offset);
  offset += 21;

  buf.writeUInt8(0, offset++);
  buf.writeUInt8(1, offset++);

  buf.writeInt32BE(0, offset);
  offset += 4;

  buf.writeInt32BE(4, offset);
  offset += 4;
  buf.write('node', offset);
  offset += 4;

  buf.writeInt32BE(1, offset);
  offset += 4;
  buf.write('i', offset++);

  new cid.ChatId(cookie)._data.copy(buf, offset);
  return buf;
}


function onRequest(data, conn) {
  if (data.method !== 'AUTH')
    return;

  if (Auth.hasOwnProperty(data.request))
    Auth[data.request].apply(Auth, arguments);
  else
    conn.write(packets.error(data.id, 404));
}

exports.request   = request;
exports.onRequest = onRequest;