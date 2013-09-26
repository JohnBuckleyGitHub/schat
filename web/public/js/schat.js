;(function(){

// This would be the place to edit if you want a different
// Base32 implementation

var alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ234567';
var alias = { o:0, i:1, l:1, s:5 };

/**
 * Build a lookup table and memoize it
 *
 * Return an object that maps a character to its
 * byte value.
 */

var lookup = function() {
    var table = {};
    // Invert 'alphabet'
    for (var i = 0; i < alphabet.length; i++) {
        table[alphabet[i]] = i;
    }
    // Splice in 'alias'
    for (var key in alias) {
        if (!alias.hasOwnProperty(key)) continue;
        table[key] = table['' + alias[key]];
    }
    lookup = function() { return table; };
    return table;
};

/**
 * A streaming encoder
 *
 *     var encoder = new base32.Encoder()
 *     var output1 = encoder.update(input1)
 *     var output2 = encoder.update(input2)
 *     var lastoutput = encode.update(lastinput, true)
 */

function Encoder() {
    var skip = 0; // how many bits we will skip from the first byte
    var bits = 0; // 5 high bits, carry from one byte to the next

    this.output = '';

    // Read one byte of input
    // Should not really be used except by "update"
    this.readByte = function(byte) {
        // coerce the byte to an int
        if (typeof byte == 'string') byte = byte.charCodeAt(0);

        if (skip < 0) { // we have a carry from the previous byte
            bits |= (byte >> (-skip));
        } else { // no carry
            bits = (byte << skip) & 248;
        }

        if (skip > 3) {
            // not enough data to produce a character, get us another one
            skip -= 8;
            return 1;
        }

        if (skip < 4) {
            // produce a character
            this.output += alphabet[bits >> 3];
            skip += 5;
        }

        return 0;
    };

    // Flush any remaining bits left in the stream
    this.finish = function(check) {
        var output = this.output + (skip < 0 ? alphabet[bits >> 3] : '') + (check ? '$' : '');
        this.output = '';
        return output;
    };
}

/**
 * Process additional input
 *
 * input: string of bytes to convert
 * flush: boolean, should we flush any trailing bits left
 *        in the stream
 * returns: a string of characters representing 'input' in base32
 */

Encoder.prototype.update = function(input, flush) {
    for (var i = 0; i < input.length; ) {
        i += this.readByte(input[i]);
    }
    // consume all output
    var output = this.output;
    this.output = '';
    if (flush) {
      output += this.finish();
    }
    return output;
};

// Functions analogously to Encoder

function Decoder() {
    var skip = 0; // how many bits we have from the previous character
    var byte = 0; // current byte we're producing

    this.output = '';

    // Consume a character from the stream, store
    // the output in this.output. As before, better
    // to use update().
    this.readChar = function(char) {
        if (typeof char != 'string'){
            if (typeof char == 'number') {
                char = String.fromCharCode(char);
            }
        }
        char = char.toUpperCase();
        var val = lookup()[char];
        if (typeof val == 'undefined') {
            // character does not exist in our lookup table
            return; // skip silently. An alternative would be:
            // throw Error('Could not find character "' + char + '" in lookup table.')
        }
        val <<= 3; // move to the high bits
        byte |= val >>> skip;
        skip += 5;
        if (skip >= 8) {
            // we have enough to preduce output
            this.output += String.fromCharCode(byte);
            skip -= 8;
            if (skip > 0) byte = (val << (5 - skip)) & 255;
            else byte = 0;
        }
    };

    this.finish = function(check) {
        var output = this.output + (skip < 0 ? alphabet[bits >> 3] : '') + (check ? '$' : '');
        this.output = '';
        return output;
    };
}

Decoder.prototype.update = function(input, flush) {
    for (var i = 0; i < input.length; i++) {
        this.readChar(input[i]);
    }
    var output = this.output;
    this.output = '';
    if (flush) {
      output += this.finish();
    }
    return output;
};

/** Convenience functions
 *
 * These are the ones to use if you just have a string and
 * want to convert it without dealing with streams and whatnot.
 */

// String of data goes in, Base32-encoded string comes out.
function encode(input) {
  return new Encoder().update(input, true);
}

// Base32-encoded string goes in, decoded data comes out.
function decode(input) {
  return new Decoder().update(input, true);
}

base32 = {
    Decoder: Decoder,
    Encoder: Encoder,
    encode: encode,
    decode: decode
};

if (typeof window !== 'undefined') {
  // we're in a browser - OMG!
  window.base32 = base32;
} else {
  module.exports = base32;
}
})();

//     uuid.js
//
//     (c) 2010-2012 Robert Kieffer
//     MIT License
//     https://github.com/broofa/node-uuid
(function() {
  var _global = this;

  // Unique ID creation requires a high quality random # generator.  We feature
  // detect to determine the best RNG source, normalizing to a function that
  // returns 128-bits of randomness, since that's what's usually required
  var _rng;

  // Node.js crypto-based RNG - http://nodejs.org/docs/v0.6.2/api/crypto.html
  //
  // Moderately fast, high quality
  if (typeof(require) == 'function') {
    try {
      var _rb = require('crypto').randomBytes;
      _rng = _rb && function() {return _rb(16);};
    } catch(e) {}
  }

  if (!_rng && _global.crypto && crypto.getRandomValues) {
    // WHATWG crypto-based RNG - http://wiki.whatwg.org/wiki/Crypto
    //
    // Moderately fast, high quality
    var _rnds8 = new Uint8Array(16);
    _rng = function whatwgRNG() {
      crypto.getRandomValues(_rnds8);
      return _rnds8;
    };
  }

  if (!_rng) {
    // Math.random()-based (RNG)
    //
    // If all else fails, use Math.random().  It's fast, but is of unspecified
    // quality.
    var  _rnds = new Array(16);
    _rng = function() {
      for (var i = 0, r; i < 16; i++) {
        if ((i & 0x03) === 0) r = Math.random() * 0x100000000;
        _rnds[i] = r >>> ((i & 0x03) << 3) & 0xff;
      }

      return _rnds;
    };
  }

  // Buffer class to use
  var BufferClass = typeof(Buffer) == 'function' ? Buffer : Array;

  // Maps for number <-> hex string conversion
  var _byteToHex = [];
  var _hexToByte = {};
  for (var i = 0; i < 256; i++) {
    _byteToHex[i] = (i + 0x100).toString(16).substr(1);
    _hexToByte[_byteToHex[i]] = i;
  }

  // **`parse()` - Parse a UUID into it's component bytes**
  function parse(s, buf, offset) {
    var i = (buf && offset) || 0, ii = 0;

    buf = buf || [];
    s.toLowerCase().replace(/[0-9a-f]{2}/g, function(oct) {
      if (ii < 16) { // Don't overflow!
        buf[i + ii++] = _hexToByte[oct];
      }
    });

    // Zero out remaining bytes if string was short
    while (ii < 16) {
      buf[i + ii++] = 0;
    }

    return buf;
  }

  // **`unparse()` - Convert UUID byte array (ala parse()) into a string**
  function unparse(buf, offset) {
    var i = offset || 0, bth = _byteToHex;
    return  bth[buf[i++]] + bth[buf[i++]] +
            bth[buf[i++]] + bth[buf[i++]] + '-' +
            bth[buf[i++]] + bth[buf[i++]] + '-' +
            bth[buf[i++]] + bth[buf[i++]] + '-' +
            bth[buf[i++]] + bth[buf[i++]] + '-' +
            bth[buf[i++]] + bth[buf[i++]] +
            bth[buf[i++]] + bth[buf[i++]] +
            bth[buf[i++]] + bth[buf[i++]];
  }

  // **`v1()` - Generate time-based UUID**
  //
  // Inspired by https://github.com/LiosK/UUID.js
  // and http://docs.python.org/library/uuid.html

  // random #'s we need to init node and clockseq
  var _seedBytes = _rng();

  // Per 4.5, create and 48-bit node id, (47 random bits + multicast bit = 1)
  var _nodeId = [
    _seedBytes[0] | 0x01,
    _seedBytes[1], _seedBytes[2], _seedBytes[3], _seedBytes[4], _seedBytes[5]
  ];

  // Per 4.2.2, randomize (14 bit) clockseq
  var _clockseq = (_seedBytes[6] << 8 | _seedBytes[7]) & 0x3fff;

  // Previous uuid creation time
  var _lastMSecs = 0, _lastNSecs = 0;

  // See https://github.com/broofa/node-uuid for API details
  function v1(options, buf, offset) {
    var i = buf && offset || 0;
    var b = buf || [];

    options = options || {};

    var clockseq = options.clockseq != null ? options.clockseq : _clockseq;

    // UUID timestamps are 100 nano-second units since the Gregorian epoch,
    // (1582-10-15 00:00).  JSNumbers aren't precise enough for this, so
    // time is handled internally as 'msecs' (integer milliseconds) and 'nsecs'
    // (100-nanoseconds offset from msecs) since unix epoch, 1970-01-01 00:00.
    var msecs = options.msecs != null ? options.msecs : new Date().getTime();

    // Per 4.2.1.2, use count of uuid's generated during the current clock
    // cycle to simulate higher resolution clock
    var nsecs = options.nsecs != null ? options.nsecs : _lastNSecs + 1;

    // Time since last uuid creation (in msecs)
    var dt = (msecs - _lastMSecs) + (nsecs - _lastNSecs)/10000;

    // Per 4.2.1.2, Bump clockseq on clock regression
    if (dt < 0 && options.clockseq == null) {
      clockseq = clockseq + 1 & 0x3fff;
    }

    // Reset nsecs if clock regresses (new clockseq) or we've moved onto a new
    // time interval
    if ((dt < 0 || msecs > _lastMSecs) && options.nsecs == null) {
      nsecs = 0;
    }

    // Per 4.2.1.2 Throw error if too many uuids are requested
    if (nsecs >= 10000) {
      throw new Error('uuid.v1(): Can\'t create more than 10M uuids/sec');
    }

    _lastMSecs = msecs;
    _lastNSecs = nsecs;
    _clockseq = clockseq;

    // Per 4.1.4 - Convert from unix epoch to Gregorian epoch
    msecs += 12219292800000;

    // `time_low`
    var tl = ((msecs & 0xfffffff) * 10000 + nsecs) % 0x100000000;
    b[i++] = tl >>> 24 & 0xff;
    b[i++] = tl >>> 16 & 0xff;
    b[i++] = tl >>> 8 & 0xff;
    b[i++] = tl & 0xff;

    // `time_mid`
    var tmh = (msecs / 0x100000000 * 10000) & 0xfffffff;
    b[i++] = tmh >>> 8 & 0xff;
    b[i++] = tmh & 0xff;

    // `time_high_and_version`
    b[i++] = tmh >>> 24 & 0xf | 0x10; // include version
    b[i++] = tmh >>> 16 & 0xff;

    // `clock_seq_hi_and_reserved` (Per 4.2.2 - include variant)
    b[i++] = clockseq >>> 8 | 0x80;

    // `clock_seq_low`
    b[i++] = clockseq & 0xff;

    // `node`
    var node = options.node || _nodeId;
    for (var n = 0; n < 6; n++) {
      b[i + n] = node[n];
    }

    return buf ? buf : unparse(b);
  }

  // **`v4()` - Generate random UUID**

  // See https://github.com/broofa/node-uuid for API details
  function v4(options, buf, offset) {
    // Deprecated - 'format' argument, as supported in v1.2
    var i = buf && offset || 0;

    if (typeof(options) == 'string') {
      buf = options == 'binary' ? new BufferClass(16) : null;
      options = null;
    }
    options = options || {};

    var rnds = options.random || (options.rng || _rng)();

    // Per 4.4, set bits for version and `clock_seq_hi_and_reserved`
    rnds[6] = (rnds[6] & 0x0f) | 0x40;
    rnds[8] = (rnds[8] & 0x3f) | 0x80;

    // Copy bytes to buffer, if provided
    if (buf) {
      for (var ii = 0; ii < 16; ii++) {
        buf[i + ii] = rnds[ii];
      }
    }

    return buf || unparse(rnds);
  }

  // Export public API
  var uuid = v4;
  uuid.v1 = v1;
  uuid.v4 = v4;
  uuid.parse = parse;
  uuid.unparse = unparse;
  uuid.BufferClass = BufferClass;

  if (_global.define && define.amd) {
    // Publish as AMD module
    define(function() {return uuid;});
  } else if (typeof(module) != 'undefined' && module.exports) {
    // Publish as node.js module
    module.exports = uuid;
  } else {
    // Publish as global (in browsers)
    var _previousRoot = _global.uuid;

    // **`noConflict()` - (browser only) to reset global 'uuid' var**
    uuid.noConflict = function() {
      _global.uuid = _previousRoot;
      return uuid;
    };

    _global.uuid = uuid;
  }
}());

(function() {
  'use strict';

  window.schat = {
    net: {},
    ui: {},
    utils: {},
    lang: {},
    page: ''
  };
})();
(function() {
  'use strict';

  var namedParam = /:\w+/g;
  var splatParam = /\*\w+/g;

  function Router() {
    this.routes  = {};
    this.leave   = null;
    this.current = '';

    // BUG: https://code.google.com/p/chromium/issues/detail?id=63040
    window.addEventListener('load', function() {
      setTimeout(function() {
        window.addEventListener('popstate', function() {
          this.checkRoutes(document.location.pathname);
        }.bind(this));
      }.bind(this), 0);
    }.bind(this));
  }

  Router.prototype.route = function(route, callback, leave) {
    route = route.replace(namedParam, '([^\/]+)').replace(splatParam, '(.*?)');
    leave = leave || null;
    this.routes["^" + route + "$"] = [callback, leave];
  };

  Router.prototype.navigate = function(url, trigger, replace) {
    if (replace === true)
      window.history.replaceState({url:url}, '', url);
    else
      window.history.pushState({url:url}, '', url);

    if (trigger !== false)
      this.checkRoutes(url);
  };

  Router.prototype.checkRoutes = function(url) {
    var regex;
    var key  = '^$';
    var args = [];

    for (var regexText in this.routes) if (this.routes.hasOwnProperty(regexText)) {
      regex = new RegExp(regexText);
      if (regex.test(url)) {
        key  = regexText;
        args = regex.exec(url).slice(1);
        break;
      }
    }

    if (this.leave !== null && this.current !== key)
      this.leave();

    this.routes[key][0].apply(window, args);
    this.leave   = this.routes[key][1];
    this.current = key;
  };

  schat.router = new Router();
})();
(function() {
  'use strict';

  var utils = window.schat.utils;

  utils.isFunction =  function(obj) {
    return typeof obj === 'function';
  };

  utils.isArray = Array.isArray || function(obj) {
    return Object.prototype.toString.call(obj) == '[object Array]';
  };

  utils.randomId = function() {
    var buffer = new Array(16);
    uuid.v4(null, buffer, 0);

    return base32.encode(buffer);
  };

  utils.format = function(text, data) {
    return text.replace(/{(\d+)}/g, function(match, number) {
      return typeof data[number] !== 'undefined' ? data[number] : match;
    });
  };
})();

(function(){
  'use strict';

  var storage = {};
  var dirty   = true;

  function put(key, data, date) {
    storage[key] = {data:data, date:date, dirty:false};
  }

  function get(key) {
    if (!storage.hasOwnProperty(key))
      return null;

    return storage[key];
  }

  function contains(key) {
    if (!storage.hasOwnProperty(key))
      return 0;

    return storage[key].date;
  }

  function open() {
    dirty = false;
  }

  function close() {
    if (dirty)
      return;

    for (var key in storage) if (storage.hasOwnProperty(key)) {
      storage[key].dirty = true;
    }

    dirty = true;
  }

  if (typeof window !== 'undefined') {
    schat.net.cache = {
      put: put,
      get: get,
      contains: contains,
      close: close,
      open: open
    };
  } else {
    exports.put      = put;
    exports.get      = get;
    exports.contains = contains;
    exports.close    = close;
    exports.open     = open;
  }
})();

(function() {
  'use strict';

  function htmlspecialchars (string, quote_style, charset, double_encode) {
    // http://kevin.vanzonneveld.net
    // +   original by: Mirek Slugen
    // +   improved by: Kevin van Zonneveld (http://kevin.vanzonneveld.net)
    // +   bugfixed by: Nathan
    // +   bugfixed by: Arno
    // +    revised by: Kevin van Zonneveld (http://kevin.vanzonneveld.net)
    // +    bugfixed by: Brett Zamir (http://brett-zamir.me)
    // +      input by: Ratheous
    // +      input by: Mailfaker (http://www.weedem.fr/)
    // +      reimplemented by: Brett Zamir (http://brett-zamir.me)
    // +      input by: felix
    // +    bugfixed by: Brett Zamir (http://brett-zamir.me)
    // %        note 1: charset argument not supported
    // *     example 1: htmlspecialchars("<a href='test'>Test</a>", 'ENT_QUOTES');
    // *     returns 1: '&lt;a href=&#039;test&#039;&gt;Test&lt;/a&gt;'
    // *     example 2: htmlspecialchars("ab\"c'd", ['ENT_NOQUOTES', 'ENT_QUOTES']);
    // *     returns 2: 'ab"c&#039;d'
    // *     example 3: htmlspecialchars("my "&entity;" is still here", null, null, false);
    // *     returns 3: 'my &quot;&entity;&quot; is still here'
    var optTemp = 0,
        i = 0,
        noquotes = false;
    if (typeof quote_style === 'undefined' || quote_style === null) {
      quote_style = 2;
    }
    string = string.toString();
    if (double_encode !== false) { // Put this first to avoid double-encoding
      string = string.replace(/&/g, '&amp;');
    }
    string = string.replace(/</g, '&lt;').replace(/>/g, '&gt;');

    var OPTS = {
      'ENT_NOQUOTES': 0,
      'ENT_HTML_QUOTE_SINGLE': 1,
      'ENT_HTML_QUOTE_DOUBLE': 2,
      'ENT_COMPAT': 2,
      'ENT_QUOTES': 3,
      'ENT_IGNORE': 4
    };
    if (quote_style === 0) {
      noquotes = true;
    }
    if (typeof quote_style !== 'number') { // Allow for a single string or an array of string flags
      quote_style = [].concat(quote_style);
      for (i = 0; i < quote_style.length; i++) {
        // Resolve string input to bitwise e.g. 'ENT_IGNORE' becomes 4
        if (OPTS[quote_style[i]] === 0) {
          noquotes = true;
        }
        else if (OPTS[quote_style[i]]) {
          optTemp = optTemp | OPTS[quote_style[i]];
        }
      }
      quote_style = optTemp;
    }
    if (quote_style & OPTS.ENT_HTML_QUOTE_SINGLE) {
      string = string.replace(/'/g, '&#039;');
    }
    if (!noquotes) {
      string = string.replace(/"/g, '&quot;');
    }

    return string;
  }

  schat.utils.encode = htmlspecialchars;
})();
(function() {
  'use strict';

  var l = {};
  l['common-signin']     = 'Sign in';
  l['common-nick']       = 'Nick';
  l['common-your-nick']  = 'Your Nick';
  l['common-email']      = 'Email';
  l['common-your-email'] = 'Your Email';
  l['common-male']       = 'Male';
  l['common-female']     = 'Female';
  l['common-rooms']      = 'Rooms';
  l['common-stats']      = 'Statistics';
  l['common-hour']       = 'Hour';
  l['common-day']        = 'Day';
  l['common-week']       = 'Week';
  l['common-month']      = 'Month';
  l['common-year']       = 'Year';

  l['oauth-header']      = 'Sign in using your account with';
  l['channels-no-title'] = 'No title';

  l['stats-stats']       = 'Now {0} online in {1}. Peak online <b>{2}</b> was <span class="timeago" data-date="{3}"></span>.';
  l['stats-users']       = function(param) { return (param === 1 ? '<b>%d</b> user' : '<b>%d</b> users'); };
  l['stats-rooms']       = function(param) { return (param === 1 ? '<b>%d</b> room' : '<b>%d</b> rooms'); };
  l['stats-title']       = 'Users online';
  l['stats-title-hour']  = 'by hour';
  l['stats-title-day']   = 'by day';
  l['stats-title-week']  = 'by week';
  l['stats-title-month'] = 'by month';
  l['stats-title-year']  = 'by year';

  l['user-not-found']      = 'User not found';
  l['user-status-offline'] = 'Offline';
  l['user-status-online']  = 'Online';
  l['user-status-away']    = 'Away';
  l['user-status-dnd']     = 'Do not disturb';
  l['user-status-free']    = 'Free for Chat';

  l.timeago = {
    prefixAgo: null,
    prefixFromNow: null,
    suffixAgo: "ago",
    suffixFromNow: "from now",
    seconds: "less than a minute",
    minute: "about a minute",
    minutes: "<b>%d</b> minutes",
    hour: "about an hour",
    hours: "about <b>%d</b> hours",
    day: "a day",
    days: "<b>%d</b> days",
    month: "about a month",
    months: "<b>%d</b> months",
    year: "about a year",
    years: "<b>%d</b> years",
    wordSeparator: " ",
    numbers: []
  };

  schat.lang.en = l;
})();
(function() {
  'use strict';

  function numpf(n, f, s, t) {
    // f - 1, 21, 31, ...
    // s - 2-4, 22-24, 32-34 ...
    // t - 5-20, 25-30, ...
    var n10 = n % 10;
    if ( (n10 == 1) && ( (n == 1) || (n > 20) ) ) {
      return f;
    } else if ( (n10 > 1) && (n10 < 5) && ( (n > 20) || (n < 10) ) ) {
      return s;
    } else {
      return t;
    }
  }

  var l = {};
  l['common-signin']     = 'Войти';
  l['common-nick']       = 'Ник';
  l['common-your-nick']  = 'Ваш ник';
  l['common-email']      = 'Email';
  l['common-your-email'] = 'Ваш Email';
  l['common-male']       = 'Мужской';
  l['common-female']     = 'Женский';
  l['common-rooms']      = 'Комнаты';
  l['common-stats']      = 'Статистика';
  l['common-hour']       = 'Час';
  l['common-day']        = 'День';
  l['common-week']       = 'Неделя';
  l['common-month']      = 'Месяц';
  l['common-year']       = 'Год';

  l['oauth-header']      = 'Войти в чат через свой аккаунт в';
  l['channels-no-title'] = 'Без заголовка';

  l['stats-stats']       = 'Сейчас {0} онлайн в {1}. Пиковый онлайн <b>{2}</b> был <span class="timeago" data-date="{3}"></span>.';
  l['stats-users']       = function(param) { return numpf(param, "<b>%d</b> пользователь", "<b>%d</b> пользователя", "<b>%d</b> пользователей"); };
  l['stats-rooms']       = function(param) { return numpf(param, "<b>%d</b> комнате", "<b>%d</b> комнатах", "<b>%d</b> комнатах"); };
  l['stats-title']       = 'Пользователи онлайн';
  l['stats-title-hour']  = 'за час';
  l['stats-title-day']   = 'за день';
  l['stats-title-week']  = 'за неделю';
  l['stats-title-month'] = 'за месяц';
  l['stats-title-year']  = 'за год';

  l['user-not-found']      = 'Пользователь не найден';
  l['user-status-offline'] = 'Не в сети';
  l['user-status-online']  = 'В сети';
  l['user-status-away']    = 'Отсутствую';
  l['user-status-dnd']     = 'Не беспокоить';
  l['user-status-free']    = 'Готов общаться';

  l.timeago = {
    prefixAgo: null,
    prefixFromNow: "через",
    suffixAgo: "назад",
    suffixFromNow: null,
    seconds: "меньше минуты",
    minute: "минуту",
    minutes: function(value) { return numpf(value, "<b>%d<b/> минуту", "<b>%d</b> минуты", "<b>%d</b> минут"); },
    hour: "час",
    hours: function(value) { return numpf(value, "<b>%d</b> час", "<b>%d</b> часа", "<b>%d</b> часов"); },
    day: "день",
    days: function(value) { return numpf(value, "<b>%d</b> день", "<b>%d</b> дня", "<b>%d</b> дней"); },
    month: "месяц",
    months: function(value) { return numpf(value, "<b>%d</b> месяц", "<b>%d</b> месяца", "<b>%d</b> месяцев"); },
    year: "год",
    years: function(value) { return numpf(value, "<b>%d</b> год", "<b>%d</b> года", "<b>%d</b> лет"); }
  };

  schat.lang.ru = l;
})();
(function() {
  'use strict';

  schat.lang.current = localStorage.getItem('language') || (navigator.language || navigator.userLanguage).substr(0, 2);

  window.tr = function(key, param) {
    var lang = schat.lang.current;
    if (!schat.lang.hasOwnProperty(lang)) {
      lang = 'en';

      if (!schat.lang.hasOwnProperty(lang))
        return key;
    }

    var value = schat.lang[lang][key];
    if (!value && lang != 'en')
      value = schat.lang.en[key];

    value = value || key;
    if (schat.utils.isFunction(value))
      return value(param).replace('%d', param);

    return value;
  };
})();
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
(function() {
  'use strict';

  SockJS.prototype.send = function(data) {
    if (this.readyState === SockJS.CONNECTING)
      throw new Error('INVALID_STATE_ERR');
    if (this.readyState === SockJS.OPEN) {
      this._transport.doSend(JSON.stringify(data));
    }
    return true;
  };

  var Client = {
    url: '/chat',
    _pending: {},
    _debug: false,

    /**
     * Запуск подключения к серверу.
     *
     * @returns {boolean}
     */
    start: function() {
      if (this !== Client)
        Client.start.call(Client);

      this._debug = !!localStorage.getItem('debug') && console && console.log && console.log.apply;

      this.socket = new SockJS(this.url);
      this.socket.addEventListener('open',    this._open.bind(this));
      this.socket.addEventListener('close',   this._close.bind(this));
      this.socket.addEventListener('message', this._message.bind(this));

      return true;
    },


    /**
     * Отправка пакета.
     *
     * @param packet
     * @param callback Фунция вызываемая при получении ответа на запрос, получает 2 аргумента оригинальный запрос и ответ на него.
     * @returns {boolean}
     */
    send: function(packet, callback) {
      if (this !== Client)
        Client.send.call(Client, packet, callback);

      var array = schat.net.request(packet);
      if (array === null)
        return false;

      if (schat.utils.isFunction(callback))
        this._pending[packet.id] = [packet, callback];

      if (this._debug)
        console.log('Client.send:', array);

      this.socket.send(array);
      return true;
    },


    get: function(path, callback) {
      if (this !== Client)
        Client.send.call(Client, path, callback);

      var exist = schat.net.cache.get(path);
      if (exist !== null && !exist.dirty) {
        callback({id:'', date:exist.date, status:200, data:exist.data}, path);
        return true;
      }

      return this.send({request:path, date:(exist ? exist.date : 0)}, callback);
    },


    /**
     * Добавление слушателя событий.
     *
     * @param eventType
     * @param listener
     */
    on: function(eventType, listener) {
      this._listeners = this._listeners || {};
      var list = this._listeners[eventType] = this._listeners[eventType] || [];
      if (list.indexOf(listener) < 0)
        list.push(listener);
    },


    /**
     * Удаление слушателей события.
     *
     * @param eventType
     * @param listener
     */
    off: function(eventType, listener) {
      if (!this._listeners || !this._listeners[eventType])
        return;

      if (!listener) {
        delete this._listeners[eventType];
        return;
      }

      var list = this._listeners[eventType];
      var i    = list.length;

      while (i--) {
        if (listener !== list[i])
          continue;

        list.splice(i, 1);
      }
    },


    /**
     * Вызов слушателей события.
     *
     * @param eventType
     * @param event
     */
    emit: function(eventType, event) {
      if (!this._listeners || !this._listeners[eventType])
        return;

      var list = this._listeners[eventType];
      for (var i = 0; i < list.length; i++)
        list[i](event);
    },


    /**
     * Обработка входящего пакета.
     *
     * @param event
     */
    _message: function(event) {
      var reply = schat.net.reply(event.data);
      if (reply === null)
        return;

      if (this._debug)
        console.log('Client._message:', reply);

      if (this._pending.hasOwnProperty(reply.id)) {
        var req = this._pending[reply.id][0];
        if (req.method === 'GET') {
          if (reply.status === 304) {
            var exists = schat.net.cache.get(req.request);
            if (exists !== null)
              schat.net.cache.put(req.request, exists.data, reply.date);
          }
          else if (reply.status === 200)
            schat.net.cache.put(req.request, reply.data, reply.date);
        }

        this._pending[reply.id][1](reply, req.request);
        delete this._pending[reply.id];
      }

      this.emit('packet', reply);

      if (reply.type === 'RES') {
        if (reply.id === this.serverId + '/parent') {
          if (reply.data[0] === this.serverId) {
            schat.net.cache.open();
            this.emit('open');
          }
          else
            schat.net.cache.close();
        }
        else
          schat.net.cache.put(reply.id, reply.data, reply.date);

        reply.status = 200;
        this.emit('res.' + reply.id, reply);
      }
    },


    _close: function() {
      if (this._debug)
        console.log('Client._close:');

      schat.net.cache.close();
      this.emit('close');

      window.setTimeout(this.start.bind(this), 2000);
    },


    _open: function() {
      if (this._debug)
        console.log('Client._open:', this.socket.protocol);

      schat.net.cache.open();
      this.serverId = localStorage.getItem('serverId');

      if (typeof this.serverId !== 'string' || this.serverId.length !== 34) {
        this.send({request:'server/server'}, function(reply) {
          if (reply.status === 200) {
            this.serverId = reply.data[0].id;
            localStorage.setItem('serverId', this.serverId);
          }

          this.emit('open');
        }.bind(this));
      }
      else
        this.emit('open');
    }
  };

  window.schat.net.client = Client;
})();


(function(){
  'use strict';

  var client = schat.net.client;
  var widget = null;

  function open() {
    if (schat.page != 'index')
      return;

    get();
  }


  /**
   * Запрос данных.
   *
   * @returns {boolean}
   */
  function get() {
    if (client.socket.readyState != 1)
      return false;

    client.on('res.' + client.serverId + '/index', rooms);
    client.get(client.serverId + '/index', rooms);

    client.on('res.' + client.serverId + '/stats', stats);
    client.get(client.serverId + '/stats', stats);
    return true;
  }


  /**
   * Обработка получения списка комнат.
   *
   * @param reply
   */
  function rooms(reply) {
    if (schat.page != 'index')
      return;

    if (reply.status === 200) {
      if (widget === null)
        widget = new schat.ui.MainWidget();

      widget.date.index = reply.date;
      widget.setRooms(reply.data);
    }

    hideLoader();
  }


  /**
   * Обработка получения статистики.
   *
   * @param reply
   */
  function stats(reply) {
    if (schat.page !== 'index')
      return;

    if (reply.status === 200) {
      if (widget === null)
        widget = new schat.ui.MainWidget();

      widget.date.stats = reply.date;
      widget.setStats(reply.data[0]);
    }

    hideLoader();
  }


  function hideLoader() {
    if (widget && widget.date.index && widget.date.stats)
      schat.ui.Loader.hide();
  }


  function route() {
    schat.page = 'index';
    schat.router.navigate('/', false, true);

    schat.ui.Loader.show();
    get();
  }


  function unload() {
    client.off('res.' + client.serverId + '/index', rooms);
    client.off('res.' + client.serverId + '/stats', stats);

    widget = null;
  }


  schat.router.route('', route, unload);
  client.on('open', open);
})();
(function(){
  'use strict';

  var Auth = {
    widget: null
  };

  /**
   * Обработка получения списка провайдеров.
   * @param reply
   */
  Auth.providers = function(reply) {
    if (schat.page !== 'signin')
      return;

    if (reply.status !== 200)
      return;

    this.widget = new schat.ui.OAuthWidget(reply.data[0], reply.data[1]);
    localStorage.setItem('auth', JSON.stringify({state:reply.data[1].substring(0, 34), secret:reply.data[2]}));
    schat.ui.Loader.hide();
  };

  /**
   * Обработка получения состояния авторизации.
   * @param reply
   */
  Auth.state = function(reply) {
    if (schat.page !== 'welcome')
      return;

    if (reply.status !== 200)
      return;

    this.widget = new schat.ui.WelcomeWidget(reply.data[0]);
    schat.ui.Loader.hide();
  };

  Auth.open = function() {
    if (schat.page == 'signin')
      this.getProviders();
    else if (schat.page == 'welcome')
      this.getState();
  };


  /**
   * Запрос списка провайдеров.
   *
   * @returns {boolean}
   */
  Auth.getProviders = function() {
    if (schat.net.client.socket.readyState != 1)
      return false;

    schat.net.client.send(
        {
          method:'AUTH',
          request:'providers',
          data:window.location.protocol + '//' + window.location.host + '/w'
        }, Auth.providers.bind(Auth));

    return true;
  };


  /**
   * Запрос состояния авторизации.
   *
   * @returns {boolean}
   */
  Auth.getState = function() {
    if (schat.net.client.socket.readyState != 1)
      return false;

    var auth = null;
    try {
      auth = JSON.parse(localStorage.getItem('auth'));
    }
    catch (e) {}

    if (auth === null) {
      schat.router.navigate('/signin');
      return false;
    }

    schat.net.client.send({method:'AUTH', request:'state', data:[auth.state, auth.secret]}, Auth.state.bind(this));
    return true;
  };


  schat.router.route('/signin', function() {
    schat.page = 'signin';

    Auth.getProviders();
    schat.ui.Loader.show();
  });


  schat.router.route('/w', function() {
    schat.page = 'welcome';
    schat.router.navigate('/welcome', true, true);
  });


  schat.router.route('/welcome', function() {
    schat.page = 'welcome';

    Auth.getState();
    schat.ui.Loader.show();
  });

  schat.net.client.on('open', Auth.open.bind(Auth));
})();
(function(){
  'use strict';

  var client = schat.net.client;
  var widget = null;

  function stats(reply) {
    if (schat.page !== 'stats')
      return;

    if (widget === null)
      widget = new schat.ui.StatsWidget();

    widget.setStats(reply.data[0]);
  }

  function get() {
    if (client.socket.readyState != 1)
      return false;

    client.on('res.' + client.serverId + '/stats', stats);
    client.get(client.serverId + '/stats', stats);
    return true;
  }


  function open() {
    if (schat.page !== 'stats')
      return;

    get();
    schat.ui.Loader.hide();
  }


  function route(period) {
    schat.page = 'stats';

    if (!period || (period !== 'hour' && period !== 'day' && period !== 'week' && period !== 'month' && period !== 'year')) {
      period = localStorage.getItem('statsPeriod');
      period = period || 'week';
      schat.router.navigate('/stats/' + period, false, true);
    }

    localStorage.setItem('statsPeriod', period);
    get();

    if (widget === null)
      widget = new schat.ui.StatsWidget();

    widget.setPeriod(period);
  }


  function unload() {
    if (widget)
      widget.clear();

    client.off('res.' + client.serverId + '/stats', stats);
    widget = null;
  }

  schat.router.route('/stats', route, unload);
  schat.router.route('/stats/:period', route, unload);
  client.on('open', open);
})();
(function(){
  'use strict';

  var PAGE   = 'users';
  var client = schat.net.client;
  var widget = null;

  function user(reply, req) {
    if (schat.page !== PAGE)
      return;

    console.log('USERE', reply, req);

    var data = reply.data[0];
    data.id  = req;

    widget.add(data);
  }


  function users(reply) {
    if (schat.page !== PAGE)
      return;

    if (widget === null)
      widget = new schat.ui.UsersWidget();

    if (reply.status === 200 && schat.utils.isArray(reply.data)) {
      var list = reply.data;
      for (var i = 0; i < list.length; i++)
        client.get(list[i], user);
    }

    schat.ui.Loader.hide();
  }


  function get() {
    if (client.socket.readyState != 1)
      return false;

    client.get(client.serverId + '/users', users);
    return true;
  }


  function open() {
    if (schat.page !== PAGE)
      return;

    get();
  }


  function route() {
    schat.page = PAGE;

    get();
  }


  function unload() {
    widget = null;
  }


  schat.router.route('/users', route, unload);
  client.on('open', open);
})();
(function(){
  'use strict';

  var PAGE   = 'user';
  var client = schat.net.client;
  var widget = null;
  var userId = '';

  function user(reply) {
    if (schat.page !== PAGE)
      return;

    if (reply.status === 200 && reply.data[0].type === 117) {
      if (widget === null)
        widget = new schat.ui.UserWidget();

      widget.setUser(reply.data[0]);
    }
    else
      error('user-not-found');
  }

  function get() {
    if (client.socket.readyState != 1)
      return false;

    client.on('res.' + userId, user);
    client.get(userId, user);
    return true;
  }


  function open() {
    if (schat.page !== PAGE)
      return;

    get();
    schat.ui.Loader.hide();
  }


  function route(id) {
    schat.page = PAGE;

    if (!id || id.length != 34) {
      error('user-not-found');
      return;
    }

    userId = id;
    get();
  }


  function unload() {
    widget = null;
  }


  function error(text) {
    schat.ui.main.textContent = '';
    schat.ui.main.setAttribute('class', 'panel-main');
    schat.ui.main.appendChild(schat.ui.createElement('div', {class:'alert alert-danger', 'data-tr':text}));
  }

  schat.router.route('/user/:id', route, unload);
  client.on('open', open);
})();
(function() {
  'use strict';

  window.schat.ui.main = document.getElementById('main');

  window.schat.ui.createElement = function(tagName, attrs, data) {
    var el = document.createElement(tagName);
    for (var attr in attrs) if (attrs.hasOwnProperty(attr)) {
      el.setAttribute(attr, attrs[attr]);
    }

    if (typeof data === 'string') {
      el.innerHTML = data;
    }
    else if (typeof data === 'object' && data.hasOwnProperty('text'))
      el.textContent = data.text;

    if (attrs && attrs.hasOwnProperty('data-tr'))
      el.innerHTML = tr(attrs['data-tr']);

    return el;
  };
})();
(function(){
  var Loader = {};

  Loader.widget = document.getElementById('loading');
  Loader.visible = true;

  Loader.show = function(timeout) {
    if (Loader.visible)
      return;

    timeout = timeout || 300;
    Loader.visible = true;

    Loader.timeout = setTimeout(function() {
      Loader.widget.style.display = 'block';
      setTimeout(function() { Loader.widget.style.opacity = 0.75; }, 0);
    }, timeout);
  };

  Loader.hide = function() {
    if (!Loader.visible)
      return;

    clearTimeout(Loader.timeout);
    Loader.visible = false;
    Loader.widget.style.opacity = 0;
    setTimeout(function() { Loader.widget.style.display = 'none'; }, 500);
  };

  window.schat.ui.Loader = Loader;
})();
(function() {
  'use strict';

  var create = schat.ui.createElement;

  function MainWidget() {
    schat.ui.main.textContent = '';
    schat.ui.main.setAttribute('class', 'panel-main');
    this.qt = typeof SimpleChat !== 'undefined';

    this.date = {
      index: 0,
      stats: 0
    };

    var panel = create('div', {class:'panel panel-default panel-rooms'});

    if (!this.qt)
      panel.appendChild(create('div', {class:'panel-heading','data-tr':'common-rooms'}));

    this.rooms = create('div', {class:'list-group'});
    panel.appendChild(this.rooms);
    schat.ui.main.appendChild(panel);

    panel = create('div', {class:'panel panel-default panel-stats'});
    if (!this.qt)
      panel.appendChild(create('div', {class:'panel-heading'}, '<a href="/stats" class="internal" data-tr="common-stats">' + tr('common-stats') + '</a>'));
    else
      panel.appendChild(create('div', {class:'panel-heading', 'data-tr':'common-stats'}));

    this.stats = panel.appendChild(create('div', {class:'panel-body'}));
    schat.ui.main.appendChild(panel);
  }


  MainWidget.prototype.setRooms = function(rooms) {
    this.rooms.textContent = '';

    for (var i = 0; i < rooms.length; i++) {
      var options = rooms[i][4];

      this.rooms.appendChild(create('div', {class:'list-group-item'},
          '<span class="badge">' + rooms[i][2] + '</span>' +
              ((options & 2) ? '<i class="schat-icon schat-icon-pin pull-right"></i>' : '') +
              ((options & 4) ? '<i class="schat-icon schat-icon-lock pull-right"></i>' : '') +
              '<h5 class="list-group-item-heading">' + this.toUrl(rooms[i][0], rooms[i][1], options) + '</h5>' +
              '<small class="list-group-item-text text-muted">' + (rooms[i][3] === '' ? tr('channels-no-title') : schat.utils.encode(rooms[i][3]) ) + '</small>'
      ));
    }
  };


  MainWidget.prototype.setStats = function(stats) {
    if (!stats)
      return;

    this.stats.innerHTML = schat.utils.format(tr('stats-stats'),
        [
          tr('stats-users', stats.users[0]),
          tr('stats-rooms', stats.rooms[0]),
          stats.users[1],
          stats.users[2]
        ]);

    $('.timeago').timeago();
  };


  MainWidget.prototype.toUrl = function(id, name, options) {
    return '<a class="internal" href="/talk/' + id + '">' + schat.utils.encode(name) + '</a>';
  };

  window.schat.ui.MainWidget = MainWidget;
})();
(function() {
  'use strict';

  var OAuthWidget = function OAuthWidget(providers, state) {
    this.widget = schat.ui.createElement('div', {class:'center-widget oauth-widget'});
    this.widget.appendChild(schat.ui.createElement('h3', {'data-tr':'oauth-header'}));

    var links = [];
    var link;

    for (var i = 0; i < providers.length; i++) {
      link = schat.ui.createElement(
          'a',
          {class:'btn btn-default', href:providers[i][3].replace('${STATE}', state)},
          '<i class="provider provider-' + providers[i][0] + '"></i> ' + providers[i][2]
      );

      links.push(link);
      this.widget.appendChild(link);
    }

    schat.ui.main.textContent = '';
    schat.ui.main.setAttribute('class', 'center-wrapper');
    schat.ui.main.appendChild(this.widget);

    this.adjustWidth(links);
  };

  OAuthWidget.prototype.adjustWidth = function(links) {
    var max = 0;
    var current = 0;

    for (var i = 0; i < links.length; i++) {
      current = links[i].offsetWidth;
      if (current > max)
        max = current;
    }

    for (i = 0; i < links.length; i++)
      links[i].style.width = max + 'px';

    this.widget.style.maxWidth = (max * 3 + 50) + 'px';
  };

  window.schat.ui.OAuthWidget = OAuthWidget;
})();
(function() {
  'use strict';

  function WelcomeWidget(info) {
    var user   = info.user  || {};
    user.name  = user.name  || '';
    user.email = user.email || '';

    this.widget = schat.ui.createElement('form', {class:'center-widget signin-widget clearfix'});
    this.widget.appendChild(schat.ui.createElement('button', {type:'button', class:'close', 'data-dismiss':'alert','aria-hidden':true}, '&times;'));

    this.widget.appendChild(schat.ui.createElement('div', {class:'form-group'},
        '<label for="nick-input" data-tr="welcome-nick">' + tr('common-nick') + '</label>' +
        '<div class="input-group">' +
          '<input type="text" class="form-control" id="nick-input" placeholder="' + tr('common-your-nick') + '" value="' + user.name + '">' +
          '<div class="input-group-btn">' +
            '<button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown"><span data-tr="common-male">' + tr('common-male') + '</span> <span class="caret"></span></button>' +
            '<ul class="dropdown-menu pull-right">' +
              '<li><a href="#" data-tr="common-male">' + tr('common-male') + '</a></li>' +
              '<li><a href="#" data-tr="common-female">' + tr('common-female') + '</a></li>' +
            '</ul>' +
          '</div>' +
        '</div>'
    ));

    this.widget.appendChild(schat.ui.createElement('div', {class:'form-group'},
        '<label for="email-input" data-tr="common-email">' + tr('common-email') + '</label>' +
        '<input type="text" class="form-control" id="email-input" placeholder="' + tr('common-your-email') + '" value="' + user.email + '">'
    ));

    this.widget.appendChild(schat.ui.createElement('button', {type:'submit', class:'btn btn-primary pull-right', 'data-tr':'common-signin'}));

    schat.ui.main.textContent = '';
    schat.ui.main.setAttribute('class', 'center-wrapper');
    schat.ui.main.appendChild(this.widget);
  }

  window.schat.ui.WelcomeWidget = WelcomeWidget;
})();
(function() {
  'use strict';

  var create = schat.ui.createElement;

  function StatsWidget() {
    schat.ui.main.textContent = '';
    schat.ui.main.setAttribute('class', 'panel-main');

    var panel = create('div', {class:'panel panel-default panel-stats'});

    var title = panel.appendChild(create('div', {class:'panel-heading'})).appendChild(create('h3'));
    title.appendChild(create('span', {'data-tr':'stats-title'}));
    title.appendChild(document.createTextNode(' '));

    var body  = panel.appendChild(create('div', {class:'panel-body'}));

    this.interval = null;
    this.subtitle = title.appendChild(create('small'));
    this.graph    = body.appendChild(create('img', {src:'/img/stats/users-day.png', class:'img-responsive img-stats-graph'}));
    this.pager    = body.appendChild(create('ul', {class:'pager'}));

    this.pager.innerHTML =
        '<li><a class="internal" href="/stats/hour" data-tr="common-hour">' + tr('common-hour') + '</a></li> ' +
        '<li><a class="internal" href="/stats/day" data-tr="common-day">' + tr('common-day') + '</a></li> ' +
        '<li><a class="internal" href="/stats/week" data-tr="common-week">' + tr('common-week') + '</a></li> ' +
        '<li><a class="internal" href="/stats/month" data-tr="common-month">' + tr('common-month') + '</a></li>';

    schat.ui.main.appendChild(panel);

    panel = create('div', {class:'panel panel-default panel-stats'});
    this.stats = panel.appendChild(create('div', {class:'panel-body'}));
    schat.ui.main.appendChild(panel);
  }


  StatsWidget.prototype.periods = [ 'hour', 'day', 'week', 'month' ];


  StatsWidget.prototype.setPeriod = function(period) {
    this.clear();
    this.subtitle.setAttribute('data-tr', 'stats-title-' + period);
    this.subtitle.textContent = tr('stats-title-' + period);
    this.graph.setAttribute('src', '/img/stats/users-' + period + '.png#' + new Date().getTime());

    var pager = this.pager.children;
    for (var i = 0; i < pager.length; i++) {
      pager[i].setAttribute('class', period === this.periods[i] ? 'disabled' : '');
    }

    this.interval = setInterval(function() {
      this.graph.setAttribute('src', '/img/stats/users-' + period + '.png#' + new Date().getTime());
    }.bind(this), 30000);
  };


  StatsWidget.prototype.setStats = schat.ui.MainWidget.prototype.setStats;


  StatsWidget.prototype.clear = function() {
    clearInterval(this.interval);
  };

  window.schat.ui.StatsWidget = StatsWidget;
})();
(function(){
  'use strict';

  function UserList(element) {
    this.el    = element;
    this.users = {};
    this.list  = [];
  }


  UserList.prototype.add = function(user) {
    this.users[user.id] = user;
    this.list.push(user.id);

    user.el = this.el.appendChild(schat.ui.createElement('li', {class:'list-group-item'},
        '<a class="internal" href="/user/' + user.id + '">' + user.name + '</a>'
    ));
  };


  UserList.prototype.sort = function() {
    this.list.sort(function(a, b) {
      a = this.users[a].name.toLowerCase();
      b = this.users[b].name.toLowerCase();

      if (a < b)
        return -1;

      if (a > b)
        return 1;

      return 0;
    }.bind(this));

    var user;
    for (var i = 0; i < this.list.length; i++) {
      user    = this.users[this.list[i]];
      user.el = this.el.appendChild(user.el);
    }
  };

  schat.ui.UserList = UserList;
})();
(function() {
  'use strict';

  var create = schat.ui.createElement;

  function UsersWidget() {
    schat.ui.main.textContent = '';
    schat.ui.main.setAttribute('class', 'panel-main');

    var panel = create('div', {class:'panel panel-default panel-users'});

    this.list = new schat.ui.UserList(panel.appendChild(create('ul', {class:'list-group'})));

    schat.ui.main.appendChild(panel);
  }


  UsersWidget.prototype.add = function(user) {
    this.list.add(user);
  };

  schat.ui.UsersWidget = UsersWidget;
})();
(function() {
  'use strict';

  var create = schat.ui.createElement;

  function UserWidget() {
    schat.ui.main.textContent = '';
    schat.ui.main.setAttribute('class', 'panel-main');

    var panel = create('div', {class:'panel panel-default panel-user'});

    var title = panel.appendChild(create('div', {class:'panel-heading'})).appendChild(create('h3'));
    this.nick = title.appendChild(create('span'));
    title.appendChild(document.createTextNode(' '));
    this.status = title.appendChild(create('small'));

    panel.appendChild(create('div', {class:'panel-body'}));

    schat.ui.main.appendChild(panel);
  }


  UserWidget.prototype.setUser = function(data) {
    this.nick.textContent = data.name;

    var status = schat.ui.user.status(data.status);
    this.status.setAttribute('data-tr', status);
    this.status.textContent = tr(status);
  };


  schat.ui.UserWidget = UserWidget;

  schat.ui.user = schat.ui.user || {};
  schat.ui.user.status = function(status) {
    status = status || 0;

    switch (status) {
      case 0:
        return 'user-status-offline';

      case 1:
        return 'user-status-online';

      case 2:
      case 3:
        return 'user-status-away';

      case 4:
        return 'user-status-dnd';

      case 5:
        return 'user-status-free';

      default:
        return '';
    }
  };
})();
$(document).ready(function() {
  schat.net.client.start();

  $('body').on('click', '.internal', function(event) {
    event.preventDefault();

    schat.router.navigate($(this).attr('href'));
  });

  schat.router.checkRoutes(document.location.pathname);
});