var proto    = require('./protocol');
Int64        = require('node-int64');
var sequence = 0;

var SockJSConnection = require('sockjs/lib/transport').SockJSConnection;
var Session          = require('sockjs/lib/transport').Session;
var Transport        = require('sockjs/lib/transport').Transport;

SockJSConnection.prototype.write = function(string) {
  return this._session.send(string);
};

Session.prototype.send = function(payload) {
  if (this.readyState !== Transport.OPEN) return false;
  this.send_buffer.push(payload);
  if (this.recv) this.tryFlush();
  return true;
};

function writeUInt64BE(buffer, offset, value) {
  var int64 = new Int64(value);
  for (var i = 8; i >= 0; i--)
    buffer[offset + i] = int64.buffer[i];
}

function readUInt64BE(buffer, offset) {
  return new Int64(buffer.readUInt32BE(offset), buffer.readUInt32BE(offset + 4)).toNumber();
}

function write(packets, options) {
  if (Object.prototype.toString.call(packets) !== '[object Array]')
    return write([packets], options);

  options  = options || proto.HUGE_PACKETS;
  var size = packets.length * 4 + 16;

  if (!(options & proto.HUGE_PACKETS))
    options |= proto.HUGE_PACKETS;

  for (var i = 0; i < packets.length; ++i)
    size += packets[i].length;

  var buf = new Buffer(size + 4);
  buf.writeInt32BE(size, 0);
  buf.writeUInt8(proto.GENERIC_TRANSPORT, 4);
  buf.writeUInt8(0x4, 5);
  buf.writeUInt8(0x1, 6);
  buf.writeUInt8(options, 7);
  writeUInt64BE(buf, 8, ++sequence);
  buf.writeInt32BE(packets.length, 16);

  var offset = 20;
  for (i = 0; i < packets.length; ++i) {
    buf.writeInt32BE(packets[i].length, offset);
    offset += 4;
  }

  for (i = 0; i < packets.length; ++i) {
    packets[i].copy(buf, offset);
    offset += packets[i].length;
  }

  return buf;
}

function read(buffer) {
  var reader = {
    packets: []
  };

  try {
    reader.type = buffer.readUInt8(0);
    if (reader.type != proto.GENERIC_TRANSPORT)
      return null;

    reader.version = buffer.readUInt8(1);
    if (reader.version != 0x4)
      return null;

    reader.options = buffer.readUInt8(3);
    var offset = 12;

    if (reader.options & proto.TIMESTAMP)
      offset += 8;

    var count = buffer.readUInt32BE(offset);
    var sizes = [];

    offset += 4;

    if (reader.options & proto.HUGE_PACKETS) {
      reader.options = reader.options ^ proto.HUGE_PACKETS;

      for (var i = 0; i < count; ++i) {
        sizes.push(buffer.readUInt32BE(offset));
        offset += 4;
      }
    }
    else {
      for (i = 0; i < count; ++i) {
        sizes.push(buffer.readUInt16BE(offset));
        offset += 2;
      }
    }

    for (i = 0; i < sizes.length; ++i) {
      reader.packets.push(buffer.slice(offset, offset + sizes[i]));
      offset += sizes[i];
    }

    return reader;
  }
  catch (e) {
    console.error(e);
    return null;
  }
}

exports.write         = write;
exports.read          = read;
exports.writeUInt64BE = writeUInt64BE;
exports.readUInt64BE  = readUInt64BE;
