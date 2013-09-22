var proto  = require('./protocol');
var ChatId = require('../id/cid.js').ChatId;

function basic(type) {
  var buf = new Buffer(5);
  buf.writeUInt16BE(type, 0);
  buf.writeUInt8(0, 2);
  buf.writeUInt16BE(proto.BASIC_HEADER, 3);

  return buf;
}


function readAuthReply(socket, buf) {
  var option = buf.readUInt16BE(3);

  if (option & 1)
    socket.serverId = new ChatId(buf.slice(5, 26)).toString();

  if (option & 2)
    socket.userId = new ChatId(buf.slice(30, 51)).toString();

  return buf.readUInt16BE(52);
}

exports.basic         = basic;
exports.readAuthReply = readAuthReply;
