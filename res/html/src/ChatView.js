/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright (c) 2008-2012 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

var Settings = {
  id: ''
};


var Utils = {
  // Обрезка строки.
  left: function(text, n) {
    if (typeof text !== 'string')
      return "";

    if (text.length < n)
      return text;

    return text.slice(0, n);
  },

  // Динамический перевод интерфейса.
  retranslate: function() {
    $("[data-tr]").each(function() {
      Utils.TR($(this).attr("data-tr"));
    });
  },

  simplified: function(text) {
    return $.trim(text.replace(/\s+/gi, ' '));
  },


  table: function (dict) {
    var table = '<table><tbody>';

    for (var key in dict) if (dict.hasOwnProperty(key)) {
      table += '<tr><td>' + Utils.tr(key) + '</td><td>' + dict[key] + '</td></tr>';
    }

    table += '</table></tbody>';
    return table;
  },

  tr: function(key) {
    return SimpleChat.translate(key);
  },

  TR: function(key) {
    $("[data-tr='" + key + "']").html(Utils.tr(key));
  },

  log: function(text) {
    var date = new Date();
    console.log(DateTime.time(date) + DateTime.seconds(date) + "." + date.getMilliseconds() + " " + text);
  },

  row: function(label, value) {
    if (typeof value !== "string" || value === "")
      return '';

    var out = '<div class="field-row field-' + label + '">';
    out += '<span class="field-row-label"><span data-tr="' + label + '">' + Utils.tr(label) + '</span>:</span> ';
    out += '<span class="field-row-value">' + value + '</span>';
    return out;
  },


  labels: function(obj) {
    var max = 0;
    var current = 0;

    obj.each(function() {
      current = $(this).width();
      if (current > max)
        max = current;
    });

    obj.width(max + 4);
  }
};


var Pages = {
  current: 0,
  onInfo: [],
  onMessages: [],

  // Преобразование кода операционной системы в строку.
  os: function(code)
  {
    if (code == 0 || code > 79)
      return "unknown";

    if (code >= 1 && code < 20)
      return "windows";

    if (code >= 20 && code < 40)
      return "osx";

    if (code == 41)
      return "ubuntu";

    if (code == 42)
      return "gentoo";

    if (code == 43)
      return "debian";

    if (code == 44)
      return "opensuse";

    if (code == 45)
      return "fedora";

    if (code >= 40)
      return "linux";

    return "unknown";
  },


  // Установка отображаемой страницы, возможные значения 0 - отображаются сообщения, 1 - отображается информация о канале.
  setPage: function(page)
  {
    SimpleChat.setTabPage(Settings.id, page);
    Pages.current = page;

    if (page == 0) {
      $("#messages").addClass("active");
      $("#info").removeClass("active");
      alignChat();

      for (var i = 0; i < Pages.onMessages.length; i++) {
        Pages.onMessages[i]();
      }
    }
    else if (page == 1) {
      $("#messages").removeClass("active");
      $("#info").addClass("active");

      for (i = 0; i < Pages.onInfo.length; i++) {
        Pages.onInfo[i]();
      }
    }
  }
};


Pages.onInfo.push(function() {
  $("#info-content > h1").html(Messages.nameTemplate(JSON.parse(SimpleChat.channel(Settings.id))));
});


// Объект сообщений.
var Messages = {
  // Добавление сообщения пользователя.
  addChannelMessage: function(json)
  {
    if ($("#" + json.Id).length) {
      Messages.updateChannelMessage(json);
      return;
    }

    var html = '<div class="container ' + json.Type + '-type" id="' + json.Id + '">';
    html += '<div class="blocks ';
    html += json.Direction;

    if (json.Status !== undefined)
      html += ' ' + json.Status;

    if (json.Command == "me")
      html += ' me-action';

    html += '">';

    html += DateTime.template(json.Date, json.Day);
    html += Messages.nameBlock(json.Author);
    html += '<span class="msg-body-block">' + json.Text + '</span>';

    html += '</div>';
    html += '</div>';

    Messages.addHintedRawMessage(html, json.Hint);
  },


  // Добавление сырого сообщения, с подсказкой по размещению.
  addHintedRawMessage: function(html, hint)
  {
    if (hint.Hint == "before") {
      if (!$("#" + hint.Id).length) {
        Messages.addRawMessage(html);
        return;
      }

      $("#" + hint.Id).before(html);
      alignChat();
    }
    else
      Messages.addRawMessage(html);
  },


  // Добавление сообщения.
  addMessage: function(json)
  {
    var func = json.Func;
    if (func == undefined)
      return;

    try {
      Messages[func](json);
    }
    catch (e) {}
  },


  // Добавление сырого сообщения.
  addRawMessage: function (html)
  {
    $('#Chat').append(html);
    alignChat();
  },


  // Добавление сервисного сообщения.
  addServiceMessage: function(json)
  {
    var html = '<div class="container ' + json.Type + '-type" id="' + json.Id + '">';
    html += '<div class="blocks ';
    if (json.Extra !== undefined)
      html += json.Extra;

    html += '">';

    html += DateTime.template(json.Date, false);
    html += Messages.nameBlock(json.Author);
    html += '<span class="msg-body-block">' + json.Text + '</span>';

    html += '</div>';
    html += '</div>';

    Messages.addRawMessage(html);
  },


  nameBlock: function(json)
  {
    var nick = Messages.nameTemplate(json);
    if (nick == '')
      return nick;

    return '<span class="nick-block">' + nick + '</span> ';
  },


  // Возвращает HTML шаблон имени канала.
  nameTemplate: function(json)
  {
    if (json === undefined)
      return '';

    var out = '<a class="nick ';
    out += json.Id + '" href="';
    out += json.Url + '">';
    out += htmlspecialchars(json.Name);
    out += '</a>';

    return out;
  },


  // Очистка страницы.
  reload: function()
  {
    if (Pages.current != 0)
      return;

    $(".container").remove();
  },


  // Обновление информации сообщения.
  updateChannelMessage: function(json)
  {
    var id = '#' + json.Id + ' > div.blocks';
    var classes = json.Direction;

    if (json.Status !== undefined)
      classes += ' ' + json.Status;

      if (json.Command == "me")
      classes += ' me-action';

    $(id).attr('class', 'blocks ' + classes);

    if (json.Date > 0) {
      var date = new Date(json.Date);
      if (json.Day === true)
        $(id + ' > .date-time-block > .day').text(DateTime.day(date));

      $(id + ' > .date-time-block > .time').text(DateTime.time(date));
      $(id + ' > .date-time-block > .seconds').text(DateTime.seconds(date));
    }
  },


  // Обновление имени канала.
  updateChannelName: function(data)
  {
    var json = JSON.parse(data);

    var a = $('a.' + json.Id);
    a.attr('href', json.Url);
    a.html(htmlspecialchars(json.Name));
  }
};


var DateTime = {
  // Возвращает день в формате день:месяц:год.
  day: function(date)
  {
    return DateTime.pad(date.getDate()) + ':' + DateTime.pad(date.getMonth() + 1) + ':' + date.getFullYear();
  },


  // Дополняет число ведущим 0 при необходимости.
  pad: function(n)
  {
    return n < 10 ? '0' + n : n
  },


  // Возвращает секунды в формате :секунды.
  seconds: function(date)
  {
    return ':' + DateTime.pad(date.getSeconds());
  },


  // Возвращает HTML шаблон времени сообщения.
  template: function(milliseconds, day)
  {
    var out = '';

    if (milliseconds > 0) {
      var date = new Date(milliseconds);
      out += '<span class="date-time-block">';
      if (day === true)
        out += '<span class="day">' + DateTime.day(date) + '</span> ';

      out += '<span class="time">' + DateTime.time(date) + '</span>';
      out += '<span class="seconds">' + DateTime.seconds(date) + '</span> ';
      out += '</span>';
    }

    return out;
  },


  // Возвращает время в формате часы:минуты.
  time: function(date)
  {
    return DateTime.pad(date.getHours()) + ':' + DateTime.pad(date.getMinutes());
  }
};


var Loader = {
  jsfiles: [],

  done: function() {
    ChatView.loadFinished();
    alignChat();
  },

  load: function(jsfiles) {
    for (var i = 0; i < jsfiles.length; i++)
      Loader.jsfiles.push(jsfiles[i]);

    for (i = 0; i < Loader.jsfiles.length; i++) {
      Loader.loadJS(Loader.jsfiles[i]);
    }
  },

  loadCSS: function(filename) {
    var e = document.createElement("link");
    e.setAttribute("rel", "stylesheet");
    e.setAttribute("href", filename);

    document.getElementsByTagName("head")[0].appendChild(e);
  },

  loadJS: function(jsfile) {
    $.ajax({
      url: jsfile,
      dataType: "script",
      isLocal: true,
      success: function() {
        Loader.remove(jsfile);
      },
      error: function() {
        Loader.remove(jsfile);
      }
    });
  },

  remove: function(jsfile) {
    var index = Loader.jsfiles.indexOf(jsfile);
    if (index === -1)
      return;

    Loader.jsfiles.splice(index, 1);
    if (Loader.jsfiles.length === 0)
      Loader.done();
  }
};


var Modal = {
};


$(document).ready(function() {
  //$.fx.off = true;

  $(window).resize(function() {
    alignChat();
  });

  $("#page-switcher-start").on("click", function(event){
    Pages.setPage(0);
  });

  $("#page-switcher-end").on("click", function(event){
    Pages.setPage(1);
  });

  var jsfiles = ChatView.jsfiles();
  if (jsfiles.length === 0) {
    Loader.done();
    return;
  }

  $('body').on('click.modal', '.modal-toggle', function ( e ) {
    if ($(this).data().hasOwnProperty('handler')) {
      try {
        Modal[$(this).data().handler](e);
      }
      catch (e) {}
    }

    $('#modal').modal();
    e.preventDefault();
  });

  $('#modal').on('hidden', function () {
    $('#modal-body *').remove();
  });

  $('#modal').on('shown', function () {
    Utils.labels($('#modal-body .field-row-label'));
  });

  Loader.load(jsfiles);
});

function alignChat() {
  var windowHeight = window.innerHeight;

  if (windowHeight > 0) {
    var contentElement = document.getElementById('Chat');
    var contentHeight = contentElement.offsetHeight;
    if (windowHeight - contentHeight > 0) {
      contentElement.style.position = 'relative';
      contentElement.style.top = (windowHeight - contentHeight - 4) + 'px';
    } else {
      contentElement.style.position = 'static';
    }
  }

  document.body.scrollTop = document.body.offsetHeight;
}


/// Включает или выключает отображение секунд.
function showSeconds(show) {
  if (show)
    $('#body').removeClass('no-seconds');
  else
    $('#body').addClass('no-seconds');
}


function showService(show) {
  if (show)
    $('#body').removeClass('no-service');
  else
    $('#body').addClass('no-service');

  alignChat();
}


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

  if (typeof string !== 'string')
    return "";

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

if (typeof ChatView === "undefined") {
  SimpleChat = {
    channel: function(id) { return "{}"; },
    translate: function(key) { return key; },
    setTabPage: function(id, page) {},
    feed: function(id, name) { return {}; }
  };

  ChatView = {
    jsfiles: function() { return []; },
    loadFinished: function() {}
  }
}
else {
  ChatView.reload.connect(Messages.reload);
  ChatView.message.connect(Messages.addMessage);
  SimpleChat.retranslated.connect(Utils.retranslate);
}
