exports.RECONNECT_TIME    = 1000;
exports.IDLE_TIME         = 20000;
exports.REPLY_TIME        = 15000;
exports.MAX_PACKET_SIZE   = 5242880;

exports.GENERIC_TRANSPORT = 0x0D;

exports.INTERNAL_PACKET   = 1;
exports.HUGE_PACKETS      = 2;
exports.TIMESTAMP         = 4;

exports.BASIC_HEADER      = 0;

exports.DELIVERY_CONFIRMATION_PACKET   = 0x7563;
exports.PROBE_SECURE_CONNECTION_PACKET = 0x7073;
exports.AUTH_REQUEST_PACKET            = 0x6152;
exports.AUTH_REPLY_PACKET              = 0x6172;
exports.JSON_PACKET                    = 0x6a73;

exports.SECURE_CONNECTION_REQUEST      = 0x029A;
exports.SECURE_CONNECTION_UNAVAILABLE  = 0x0309;
exports.SECURE_CONNECTION_AVAILABLE    = 0x002A;