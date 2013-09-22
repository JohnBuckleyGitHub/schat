var net      = require('net');
var util     = require('util');
var startTls = require('starttls').startTls;

var proto    = require('./protocol');
var tr       = require('./transport');
var packet   = require('./packet');

var IDLING          = 1;
var WAITING_REPLY   = 2;
var RECONNECTING    = 3;

function ClientSocket(options)
{
  if (!(this instanceof ClientSocket))
    return new ClientSocket(options);

  this._timer = null;

  net.Socket.call(this, options);
  this._setTimerState(IDLING);

  this.on('connect', this._onConnect.bind(this));
  this.on('error',   this._onError.bind(this));
  this.on('close',   this._onClose.bind(this));

  this._connected     = false;
  this._options       = {};
  this._buffer        = new Buffer(0);
  this._nextBlockSize = 0;
  this._securePair    = null;
}


util.inherits(ClientSocket, net.Socket);


ClientSocket.prototype._onConnect = function()
{
  console.log('ClientSocket._onConnect', this._securePair);
  this.on('data', this._onData.bind(this));

  clearTimeout(this._retry);
  this._buffer        = new Buffer(0);
  this._nextBlockSize = 0;
  this._connected     = true;

  this._setTimerState(IDLING);
  this.setNoDelay(true);

  var header = packet.basic(proto.PROBE_SECURE_CONNECTION_PACKET);
  var buf    = new Buffer(header.length + 2);
  header.copy(buf);
  buf.writeUInt16BE(proto.SECURE_CONNECTION_REQUEST, header.length);

  this.write(tr.write(buf, proto.INTERNAL_PACKET));
};


ClientSocket.prototype._onData = function(data) {
  if (this._buffer.length === 0)
    this._buffer = new Buffer(data);
  else
    this._buffer = Buffer.concat([this._buffer, data]);

  while (true) {
    if (this._nextBlockSize === 0) {
      if (this._buffer.length < 4)
        break;

      this._nextBlockSize = this._buffer.readInt32BE(0);

      if (this._nextBlockSize < 16 || this._nextBlockSize > proto.MAX_PACKET_SIZE) {
        this.end();
        return;
      }
    }

    if (this._buffer.length < this._nextBlockSize)
      break;

    var reader = tr.read(this._buffer.slice(4, this._nextBlockSize + 4));
    if (reader !== null && reader.packets.length) {
      if (reader.options & proto.INTERNAL_PACKET) {
        this._readInternal(reader.packets[0]);
      }
      else {
        for (var i = 0; i < reader.packets.length; i++) {
          var buf  = reader.packets[i];
          var type = buf.readUInt16BE(0);
          if (type == proto.JSON_PACKET) {
            var text = buf.toString('utf8', 9, 9 + buf.readUInt32BE(5));
            console.log(text);
            var json = null;
            try {
              json = JSON.parse(text);
            }
            catch (e) {}

            if (json !== null)
              this.emit('json', json);
          }
          else if (type == proto.AUTH_REPLY_PACKET) {
            this.emit('auth-reply', packet.readAuthReply(this, buf));
          }
        }
      }
    }

    this._buffer = this._buffer.slice(this._nextBlockSize + 4);
    this._nextBlockSize = 0;
  }
};


ClientSocket.prototype._setTimerState = function(state) {
  clearTimeout(this._timer);

  if (state == IDLING) {
    this._timer = setTimeout(function() {

      if (this._connected) {
        this.send(new Buffer(0), proto.INTERNAL_PACKET);
        this._setTimerState(WAITING_REPLY);
      }
      else
        this.destroy();

    }.bind(this), proto.IDLE_TIME);
  }
  else if (state == WAITING_REPLY) {
    this._timer = setTimeout(function() {

      if (this._connected)
        this.end();

    }.bind(this), proto.REPLY_TIME);
  }
  else if (state == RECONNECTING) {
    this._timer = setTimeout(function() {

      if (!this._connected)
        this.connect(this._options);

    }.bind(this), proto.RECONNECT_TIME);
  }
};


ClientSocket.prototype._onError = function(error)
{
  console.error('ClientSocket._onError', error);

  this._securePair = null;
  this._connected  = false;
};


ClientSocket.prototype._onClose = function(had_error)
{
  console.warn('ClientSocket._onClose', had_error);

  this._securePair = null;
  this._connected  = false;
  this._setTimerState(RECONNECTING);
};


ClientSocket.prototype._readInternal = function _readInternal(packet)
{
  this._setTimerState(IDLING);
  if (!packet.length)
    return;

  var type = packet.readUInt16BE(0);

  if (type == proto.PROBE_SECURE_CONNECTION_PACKET) {
    var option = packet.readUInt16BE(5);

    if (option == proto.SECURE_CONNECTION_AVAILABLE) {
      var securePair = startTls(this, function() {

        this._securePair = securePair;
        securePair.cleartext.on('data', this._onData.bind(this));
        securePair.cleartext.on('error', function() {});

        this.emit('auth-request');
      }.bind(this));
    }
    else if (option == proto.SECURE_CONNECTION_UNAVAILABLE) {
      this.emit('auth-request');
    }
  }
};


ClientSocket.prototype.connect = function(options, cb)
{
  this._setTimerState(IDLING);
  var args = net._normalizeConnectArgs(arguments);
  this._options = args[0];

  this.constructor.super_.prototype.connect.apply(this, args);
};


ClientSocket.prototype.send = function(data, options) {
  if (this._securePair === null)
    this.write(tr.write(data, options));
  else
    this._securePair.cleartext.write(tr.write(data, options));
};

exports.ClientSocket = ClientSocket;
