var events     = require('events');
var util       = require('util');
var packet     = require('./packet');
var auth       = require('./auth');
var packets    = require('./packets');
var ChatClient = require('./client').ChatClient;
var ChatId     = require('../id/cid').ChatId;
var users      = require('./users');
var core       = require('../core');

function ChatServer() {
  events.EventEmitter.call(this);

  core.parent = new ChatClient();

  core.sockjs.on('connection', function(conn) {

    if (conn.remoteAddress === '127.0.0.1' && conn.headers.hasOwnProperty('x-forwarded-for'))
      conn.xForwardedFor = conn.headers['x-forwarded-for'];

    conn.guest     = true;
    conn.channelId = new ChatId().toString();

    conn.on('data', function(message) {
      this._onData(conn, message);
    }.bind(this));

    users.add(conn);

  }.bind(this));

  this.on('packet.REQ', auth.onRequest);
}


util.inherits(ChatServer, events.EventEmitter);


ChatServer.prototype.init = function() {
  require('./proxy').init();

  if (core.config.stats === true)
    require('../rrd-stats').init();

  core.parent.connect();
};


ChatServer.prototype._onData = function(conn, message) {
  var data = packets.request(message);
  if (data === null)
    return;

  if (typeof data.request !== 'string') {
    conn.write(packets.error(data.id, 400));
    return;
  }

  var event = 'packet.' + data.type;

  if (!this.listeners(event).length) {
    conn.write(packets.error(data.id, 501));
    return;
  }

  this.emit(event, data, conn);
};


exports.ChatServer = ChatServer;
