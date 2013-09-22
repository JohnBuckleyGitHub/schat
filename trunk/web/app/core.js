exports.config  = null;
exports.express = null;
exports.parent  = null;
exports.server  = null;
exports.sockjs  = null;

exports.serverId = function() {
  if (exports.parent === null)
    return 'server';

  return exports.parent._socket.serverId;
};
