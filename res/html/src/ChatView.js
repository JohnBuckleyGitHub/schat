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

var channelId = '';

// Объект сообщений.
var Messages = {
  // Добавление сообщения пользователя.
  addChannelMessage: function(json, hint)
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

    Messages.addHintedRawMessage(html, hint);
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
  addMessage: function(data, hint)
  {
    var json = JSON.parse(data);
    var func = json.Func;
    if (func == undefined)
      return;

    Messages[func](json, JSON.parse(hint));
  },


  // Добавление сырого сообщения.
  addRawMessage: function (html)
  {
    $('#Chat').append(html);
    alignChat();
  },


  // Добавление сервисного сообщения.
  addServiceMessage: function(json, hint)
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


$(document).ready(function() {
  //$.fx.off = true;

  $(window).resize(function() {
    alignChat();
  });

  $("#page-switcher-start").on("click", function(event){
    setPage(0);
  });

  $("#page-switcher-end").on("click", function(event){
    setPage(1);
  });

  var timeoutID;
  $('#topic-wrapper').hover(
    function () {
      window.clearTimeout(timeoutID);
      $('#body').removeClass("no-topic-author");
    },
    function () {
      timeoutID = window.setTimeout(function() {
        $('#body').addClass("no-topic-author");
      }, 2000);
    }
  );

  alignChat();
});


function setChannelId(id)
{
  channelId = id;
}


// Установка отображаемой страницы, возможные значения 0 - отображаются сообщения, 1 - отображается информация о канале.
function setPage(page)
{
  if (page == 0) {
    $("#messages").addClass("active");
    $("#info").removeClass("active");
    alignChat();
  }
  else if (page == 1) {
    $("#messages").removeClass("active");
    $("#info").addClass("active");
    reloadInfo();
  }

  SimpleChat.setTabPage(channelId, page);
}


function reloadInfo()
{
  $("#info-content > h1").html(Messages.nameTemplate(JSON.parse(SimpleChat.channel(channelId))));
}


function loadJS(filename)
{
  var e = document.createElement("script");
  e.setAttribute("type", "text/javascript");
  e.setAttribute("src", filename);

  document.getElementsByTagName("head")[0].appendChild(e);
}


function loadCSS(filename)
{
  var e = document.createElement("link");
  e.setAttribute("rel", "stylesheet");
  e.setAttribute("type", "text/css");
  e.setAttribute("href", filename);

  document.getElementsByTagName("head")[0].appendChild(e);
}


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
