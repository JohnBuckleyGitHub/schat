var config       = require('../../config.json');
var ClientSocket = require('./socket').ClientSocket;
var events       = require('events');
var util         = require('util');
var auth         = require('./auth');
var proto        = require('./protocol');
var packet       = require('./packet');
var packets      = require('./packets');
var ChatId       = require('../id/cid').ChatId;
var cache        = require('./memory-cache');

function ChatClient(options) {
  events.EventEmitter.call(this);

  this._pending = {};
  this._socket = new ClientSocket(options);
  this._socket.on('auth-request', this._onAuthRequest.bind(this));
  this._socket.on('json', this._onJSON.bind(this));
}


util.inherits(ChatClient, events.EventEmitter);


ChatClient.prototype.connect = function() {
  this._socket.connect(config.remote);
};


ChatClient.prototype.send = function(packet, callback, type) {
  packet.id = packet.id || ChatId.messageId();
  type = type || 'REQ';

  if (typeof callback === 'function')
    this._pending[packet.id] = callback;

  this.sendJSON(JSON.stringify(packets.write(packet, type)));
};


/**
 * Запрос данных с вышестоящего сервера, в случае если данные найдены в кэше, запрос к серверу произведён не будет.
 *
 * @param packet
 * @param callback
 * @param type
 */
ChatClient.prototype.get = function(packet, callback, type) {
  packet.id = packet.id || ChatId.messageId();
  type = type || 'REQ';

  var exist = cache.get(packet.request);
  if (exist !== null && !exist.dirty) {
    callback({id:packet.id, date:exist.date, status:200, data:exist.data});
    return;
  }

  if (typeof callback === 'function')
    this._pending[packet.id] = callback;

  this.sendJSON(JSON.stringify(packets.write(packet, type)));
};


/**
 * Отправка JSON данных на сервер.
 *
 * @param json
 */
ChatClient.prototype.sendJSON = function(json) {
  console.log(json);

  var header = packet.basic(proto.JSON_PACKET);
  var offset = header.length;
  var size   = Buffer.byteLength(json);
  var buf    = new Buffer(offset + 4 + size);

  header.copy(buf);
  buf.writeInt32BE(size, offset);
  buf.write(json, offset + 4);

  this._socket.send(buf);
};


ChatClient.prototype._onAuthRequest = function() {
  this._socket.send(auth.request(config.cookie));
};


ChatClient.prototype._onJSON = function(json) {
  var data = packets.read(json);
  if (data === null)
    return;

  if (this._pending.hasOwnProperty(data.id)) {
    this._pending[data.id](data);
    delete this._pending[data.id];
  }

  this.emit('packet.' + data.type, data);
};

exports.ChatClient = ChatClient;
