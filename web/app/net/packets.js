(function(){
  /**
   * Форматы пакетов.
   */
  var formats = {};
  formats.REQ = [ 'type', 'id', 'date', 'method', 'request', 'headers', 'data' ];
  formats.REP = [ 'type', 'id', 'date', 'status', 'headers', 'data' ];
  formats.RES = [ 'type', 'id', 'date', 'headers', 'data' ];

  /**
   * Значения по умолчанию.
   */
  var defaults = {
    date: 0,
    status:  200,
    method:  'GET',
    headers: {}
  };

  /**
   * Универсальная функция записи пакета.
   */
  var write = function writePacket(packet, type) {
    if (typeof packet != 'object')
      return null;

    packet.type = packet.type || type;
    if (!formats.hasOwnProperty(packet.type))
      return null;

    var result = [ packet.type ];
    var format = formats[packet.type];
    var key;

    if (typeof schat !== 'undefined')
      packet.id = packet.id || schat.utils.randomId();

    for (var i = 1; i < format.length - 1; i++) {
      key = format[i];
      packet[key] = packet[key] || defaults[key];
      result.push(packet[key]);
    }

    if (Object.prototype.toString.call(packet.data) == '[object Array]')
      result.push.apply(result, packet.data);
    else if (typeof packet.data !== 'undefined')
      result.push(packet.data);
    else
      result.push(null);

    return result;
  };


  /**
   * Универсальная функция чтения пакета.
   */
  var read = function readPacket(packet) {
    if (Object.prototype.toString.call(packet) != '[object Array]' || packet.length < 4 || !formats.hasOwnProperty(packet[0]))
      return null;

    var result = { type:packet[0], id:packet[1] };
    var format = formats[result.type];
    var key;

    for (var i = 2; i < format.length - 1; i++) {
      key = format[i];
      result[key] = packet[i] || defaults[key];
    }

    packet.splice(0, format.length - 1);
    result.data = packet;

    return result;
  };


  if (typeof window !== 'undefined') {
    schat.net.formats = formats;
    schat.net.request = function(packet) { return write(packet, 'REQ'); };
    schat.net.reply   = function(packet) { return read(packet); };
  } else {
    exports.formats = formats;
    exports.request = function(packet) { return read(packet); };
    exports.reply   = function(packet) { return write(packet, 'REP'); };
    exports.write   = write;
    exports.read    = read;

    exports.error = function(id, status, data) {
      return write({id:id, status:status, data:data}, 'REP');
    };
  }
})();