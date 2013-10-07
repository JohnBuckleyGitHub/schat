/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright (c) 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

var FEED_METHOD_GET           = 'get';
var FEED_METHOD_PUT           = 'put';
var FEED_METHOD_POST          = 'post';
var FEED_METHOD_DELETE        = 'delete';

var FEED_NAME_ACL             = 'acl';
var FEED_NAME_CONSOLE         = 'console';
var FEED_NAME_HOSTS           = 'hosts';
var FEED_NAME_INFO            = 'info';
var FEED_NAME_LIST            = 'list';
var FEED_NAME_MESSAGES        = 'messages';
var FEED_NAME_PROFILE         = 'profile';
var FEED_NAME_SERVER          = 'server';
var FEED_NAME_USER            = 'user';
var FEED_NAME_USERS           = 'users';

var SERVER_FEED_AUTH_KEY      = 'auth';
var SERVER_FEED_AUTH_REQ      = 'server/auth';
var SERVER_FEED_POLICY_KEY    = 'policy';
var SERVER_FEED_POLICY_REQ    = 'server/policy';
var SERVER_FEED_OAUTH_KEY     = 'oauth';
var SERVER_FEED_OAUTH_REQ     = 'server/oauth';

var CONSOLE_FEED_ME_KEY       = 'me';
var CONSOLE_FEED_ME_REQ       = 'console/me';

var INFO_FEED_LOGGING_KEY     = 'logging';
var INFO_FEED_LOGGING_REQ     = 'info/logging';
var INFO_FEED_PINNED_KEY      = 'pinned';
var INFO_FEED_PINNED_REQ      = 'info/pinned';
var INFO_FEED_SUDO_REQ        = 'info/sudo';
var INFO_FEED_TITLE_REQ       = 'info/title';
var INFO_FEED_VISIBILITY_REQ  = 'info/visibility';

var ACL_FEED_HEAD_OTHER_REQ   = 'acl/head/other';
var ACL_FEED_HEAD_OWNER_REQ   = 'acl/head/owner';
var ACL_FEED_HEAD_MASK_REQ    = 'acl/head/mask';

var LIST_FEED_ID_REQ          = 'list/id';

var Settings = {
  id: '',
  status: '',
  scroll: false,
  scrollTo: null,
  autoscroll: (typeof SimpleChat !== 'undefined' ? SimpleChat.value('AutoScroll') : true),

  getId: function() {
    if (Settings.id == '')
      Settings.id = ChatView.getId();

    return Settings.id;
  },


  changed: function(key, value) {
    if (key == 'AutoScroll') {
      Settings.autoscroll = value;
      if (value === true)
        alignChat();
    }
  }
};


window.Utils = window.Utils || {};


// Обрезка строки.
Utils.left = function(text, n) {
  if (typeof text !== 'string')
    return '';

  if (text.length < n)
    return text;

  return text.slice(0, n);
};


Utils.simplified = function(text) { return $.trim(text.replace(/\s+/gi, ' ')); };


Utils.table = function (dict) {
  var table = '<table><tbody>';

  for (var key in dict) if (dict.hasOwnProperty(key)) {
    table += '<tr><td>' + Utils.tr(key) + '</td><td>' + dict[key] + '</td></tr>';
  }

  table += '</table></tbody>';
  return table;
};


Utils.row = function(label, value) {
  if (typeof value !== 'string' || value === '')
    return '';

  return '<div class="field-row field-' + label + '">' +
           '<span class="field-row-label"><span data-tr="' + label + '">' + tr(label) + '</span>:</span> ' +
           '<span class="field-row-value">' + value + '</span>' +
         '</div>';
};


/**
 * Установка ширины элементов, равной максимальной ширине одного из элементов.
 *
 * \param obj jQuery объект.
 */
Utils.adjustWidth = function(obj) {
  if (!obj.length || obj.width() < 1 || !obj.is(':visible'))
    return;

  obj.css('width', '');

  var max = 0;
  var current = 0;

  obj.each(function() {
    current = $(this).width();
    if (current > max)
      max = current;
  });

  obj.width(max + 4);
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

    if (code == 79)
      return "android";

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
      $('#page-switcher-start').hide();
      $('#page-switcher-end').show();
      $("#messages").addClass("active");
      $("#info").removeClass("active");
      alignChat();

      for (var i = 0; i < Pages.onMessages.length; i++)
        Pages.onMessages[i]();
    }
    else if (page == 1) {
      $('#page-switcher-start').show()
      $('#page-switcher-end').hide();
      $("#messages").removeClass("active");
      $("#info").addClass("active");

      for (i = 0; i < Pages.onInfo.length; i++) {
        Pages.onInfo[i]();
      }
    }
  }
};


Pages.onInfo.push(function() {
  var channel = SimpleChat.channel(Settings.getId());
  if (channel !== null)
    $('#channel-name').html(Messages.nameTemplate(channel));
});


/*
 * Объект сообщений.
 */
var Messages = {
  onAdd: [],
  onBulkAdd: [],
  unhandled: [],
  added: [],    // Список идентификаторов добавленных сообщений.

  /*
   * Вызов хуков добавления сообщения.
   */
  add: function(id) {
    Messages.added.push(id);
  },

  feed: function(json) {
    if (json.status == 200 && json.feed == FEED_NAME_MESSAGES && json.cmd == FEED_METHOD_DELETE) {
      var path = json.name.split('/', 2);
      if (path.length == 2) {
        var container = document.getElementById(path[1]);
        if (container !== null) {
          try {
            var block = container.firstChild;
            block.classList.add('removed');
            block.children[2].innerHTML = '<span class="message-removed" data-tr="message-removed">' + Utils.tr('message-removed') + '</span> <i class="message-trash"></i>';
          }
          catch (e) { console.error(e); }
        }
      }
    }
  },

  /*
   * Установка идентификатора сообщения.
   */
  setMessageId: function(container, json) {
    if (json.hasOwnProperty('OID')) {
      container.id = json.OID;
      container.setAttribute('data-mdate', json.MDate);
    }
    else
      container.id = json.Id;
  },


  /*
   * Добавление сообщения пользователя.
   */
  addChannelMessage: function(json)
  {
    var container = null;

    if (json.hasOwnProperty('InternalId')) {
      container = document.getElementById(json.InternalId);
      if (container !== null) {
        Messages.updateChannelMessage(container, json);
        return;
      }
    }

    if (json.hasOwnProperty('OID'))
      container = document.getElementById(json.OID);
    else
      container =  document.getElementById(json.Id);

    if (container !== null) {
      Messages.updateChannelMessage(container, json);
      return;
    }

    var block = document.createElement('div');
    Messages.setMessageId(block, json);
    block.setAttribute('class', 'cont ' + json.Type + '-type');
    block.setAttribute('data-time', json.Date);

    var html = '<div class="blocks ';
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

    block.innerHTML = html;

    Messages.addHintedRawMessage(block, json.Hint);

    if (json.hasOwnProperty('Status') && json.Status == 'undelivered')
      return;

    ChatView.setLastMessage(json.Date);
  },


  /*
   * Добавление сырого сообщения, с подсказкой по размещению.
   */
  addHintedRawMessage: function(block, hint)
  {
    if (hint.Hint == 'before') {
      var before = document.getElementById(hint.Id);
      if (before !== null) {
        before.parentNode.insertBefore(block, before);
        Messages.add(block.id);
        return;
      }
    }

    Messages.addRawMessage(block, hint.Day);
  },


  /*
   * Добавление сообщения.
   */
  addMessage: function(json)
  {
    var func = json.Func;
    if (func == undefined)
      return;

    if (!Messages.hasOwnProperty(func))
      return;

    try {
      if (json.Type !== 'unhandled')
        Messages.day(json.Hint.Day);

      Messages[func](json);
    }
    catch (e) {
      console.error(e);
    }
  },


  addMessages: function(messages)
  {
    if (!messages.length)
      return;

    var self     = Messages;
    var scroll   = Settings.scroll;
    var scrollTo = Settings.scrollTo;

    Settings.scroll   = false;
    Settings.scrollTo = null;

    for (var i = 0; i < messages.length; i++)
      self.addMessage(messages[i]);

    for (i = 0; i < self.added.length; i++) {
      for (var j = 0; j < self.onAdd.length; j++)
        self.onAdd[j](self.added[i]);
    }

    for (i = 0; i < self.onBulkAdd.length; i++) {
      self.onBulkAdd[i](self.added);
    }

    Settings.scroll = scroll;

    if (self.added.length) {
      self.added.length = 0;
    }
    else if (Settings.scrollTo === null)
      Settings.scrollTo = scrollTo;

    alignChat();

    if (Settings.scrollTo !== null)
      document.getElementById(Settings.scrollTo).scrollIntoView();
  },


  /*
   * Добавление сырого сообщения.
   */
  addRawMessage: function (block, day)
  {
    window.Chat.appendChild(block);
    Messages.add(block.id);
  },


  // Добавление сервисного сообщения.
  addServiceMessage: function(json)
  {
    var block = document.createElement('div');
    block.id = json.Id;
    block.setAttribute('class', 'cont ' + json.Type + '-type');
    block.innerHTML = '<div class="blocks ' + (json.Extra !== undefined ? json.Extra : '') + '">' +
                        DateTime.template(json.Date, false) +
                        Messages.nameBlock(json.Author) +
                        '<span class="msg-body-block">' + json.Text + '</span>' +
                      '</div>';

    Messages.addRawMessage(block, json.Hint.Day);
  },


  /*
   * Добавление нового уведомления.
   */
  addAlertMessage: function(json) {
    var block = document.createElement('div');
    block.id = json.Id;
    block.setAttribute('class', 'cont ' + json.Type + '-type');
    block.innerHTML = '<div class="blocks ' + (json.Extra !== undefined ? json.Extra : '') + '">' +
                        '<div class="alert-box">' +
                          '<div class="alert-box-icon"></div>' +
                          '<div class="alert-box-body">' + json.Text + '</div>' +
                          '<div class="alert-box-buttons">' +
                            '<a class="alert-box-close" href="#" data-id="' + json.Id + '"></a>' +
                          '</div>' +
                        '</div>' +
                      '</div>';

    Messages.addRawMessage(block, json.Hint.Day);
  },


  /*
   * Обработка не поддерживаемых сообщений.
   */
  addUnhandledMessage: function(json) {
    for (var i = 0; i < Messages.unhandled.length; i++)
      Messages.unhandled[i](json);
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

    return '<a class="nick ' + json.Id + ' color-' + json.Color +  '" href="' + json.Url + '">'
      + schat.utils.encode(json.Name)  + '</a>';
  },


  /*
   * Очистка страницы.
   */
  reload: function()
  {
    if (Pages.current != 0)
      return;

    $('.cont').remove();
    alignChat();
  },


  /*
   * Обновление сообщения.
   */
  updateChannelMessage: function(container, json)
  {
    Messages.setMessageId(container, json);

    var classes = json.Direction;
    if (json.Status !== undefined)
      classes += ' ' + json.Status;

      if (json.Command == 'me')
      classes += ' me-action';

    var block = container.firstChild;
    block.setAttribute('class', 'blocks ' + classes);

    if (json.Date > 0 && container.getAttribute('data-time') != json.Date) {
      var date = new Date(json.Date);

      block.firstChild.innerHTML = '<span class="time">' + DateTime.time(date) + '</span><span class="seconds">' + DateTime.seconds(date) + '</span> ';
      container.setAttribute('data-time', json.Date);
      ChatView.setLastMessage(json.Date);
    }
  },


  /*
   * Создание при необходимости блока с сообщениями за 1 день.
   * Функция заглушка, реальная функциональность реализована в плагине History.
   */
  day: function(day)
  {
  },


  rename: function(data)
  {
    var a = $('a.' + data.Id);
    a.attr('href', data.Url);
    a.html(schat.utils.encode(data.Name));
  },

  recolor: function(data)
  {
    var a = $('a.' + data.Id);
    a.removeClassRegEx(/^color-/);
    a.addClass('color-' + data.Color);
  }
};


var DateTime = {
  /*
   * Возвращает день в формате день:месяц:год.
   *
   * \param date объект даты.
   */
  day: function(date) {
    return DateTime.pad(date.getDate()) + '.' + DateTime.pad(date.getMonth() + 1) + '.' + date.getFullYear();
  },

  /*
   * Возвращает день в формате год_месяц_день.
   *
   * \param date объект даты.
   */
  dayId: function(date) {
    return date.getFullYear() + '_' + DateTime.pad(date.getMonth() + 1) + '_' + DateTime.pad(date.getDate());
  },


  /*
   * Дополняет число ведущим 0 при необходимости.
   */
  pad: function(n) {
    return n < 10 ? '0' + n : n
  },


  /*
   * Возвращает секунды в формате :секунды.
   *
   * \param date объект даты.
   */
  seconds: function(date) {
    return ':' + DateTime.pad(date.getSeconds());
  },


  /*
   * Возвращает HTML шаблон времени сообщения.
   *
   * \param milliseconds Время в виде милисекунд.
   * \param day          true если необходимо показать день сообщения.
   */
  template: function(milliseconds, day) {
    if (milliseconds > 0) {
      var date = new Date(milliseconds);
      var out = '<span class="date-time-block">';
      if (day === true)
        out += '<span class="day">' + DateTime.day(date) + '</span> ';

      out += '<span class="time">' + DateTime.time(date) + '</span>' +
             '<span class="seconds">' + DateTime.seconds(date) + '</span> </span>';

      return out;
    }

    return '';
  },


  /*
   * Возвращает время в формате часы:минуты.
   *
   * \param date объект даты.
   */
  time: function(date) {
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

  loadJS: function(jsfile, success) {
    $.ajax({
      url: jsfile,
      dataType: "script",
      isLocal: true,
      success: function() {
        Loader.remove(jsfile);

        if (typeof success == 'function')
          success();
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
  },


  /*
   * Служебный класс для отображения состояния загрузки.
   */
  spinner: {
    tasks: [],
    current: null,

    add: function(name) {
      if (Loader.spinner.current !== null && Loader.spinner.current == name)
        return;

      var key = name.split('/')[0];
      Loader.spinner.current = name;
      $('#loading-text').html(Utils.tr(key));
      $('#loading-text').attr('data-tr', key);
      $('#loading-body').fadeIn('fast');

      var tasks = Loader.spinner.tasks;
      var index = tasks.indexOf(name);
      if (index != -1)
        tasks.splice(index, 1);

      tasks.unshift(name);
    },


    remove: function(name) {
      Loader.spinner.current = null;

      var tasks = Loader.spinner.tasks;
      var index = tasks.indexOf(name);
      if (index == -1)
        return;

      tasks.splice(index, 1);

      if (index == 0) {
        Loader.spinner.current = null;
        if (tasks.length)
          Loader.spinner.add(tasks[0]);
        else
          $('#loading-body').fadeOut('fast');
      }
    },


    /*
     * Подготовка фида к чтению.
     * Если статус равен 302 запускается спинер, в остальных случаях он удаляется.
     *
     * \param name   имя фида.
     * \param status статус запроса.
     *
     * \return true если статус раверн 200 или 300.
     */
    prepare: function(name, status) {
      if (status == 302) {
        Loader.spinner.add('loading/' + name);
        return true;
      }

      Loader.spinner.remove('loading/' + name);
      return status == 200 || status == 300;
    }
  }
};


/*
 * Управление модальным диалогом.
 */
var Modal = {
  current: null, // Текущий диалог.
  create:  {},   // Функции создания.
  hidden:  {},   // Функции, вызывающиеся после скрытия диалога.
  shown:   {},   // Функции, вызывающиеся после показа диалога.

  init: function() {
    var body  = $('body');

    body.on('click.modal', '.modal-toggle', function (event) {
      event.preventDefault();
      var handler = $(this).attr('data-handler');

      if (handler !== undefined && Modal.create.hasOwnProperty(handler)) {
        if (Modal.create[handler](event) === false)
          return;

        Modal.current = handler;
      }

      $(schat.ui.modal.element).modal();
    });

    body.on('click', '[data-dismiss="alert"]', function(event) {
      event.preventDefault();
      $(this).parent().remove();
      alignChat();
    });
  }
};


$(document).ready(function() {
  window.Chat = document.getElementById('Chat');

  $(window).resize(alignChat);

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

  $('body').on('click', '.alert-box-close', function(event) {
    event.preventDefault();
    $('#' + $(this).attr('data-id')).remove();
    alignChat();
  });

  Modal.init();
  Loader.load(jsfiles);
});

function alignChat() {
  if (!Settings.scroll)
    return;

  var windowHeight = window.innerHeight;

  if (windowHeight > 0) {
    var contentElement = window.Chat;
    var contentHeight = contentElement.offsetHeight;
    if (windowHeight - contentHeight > 0) {
      contentElement.style.position = 'relative';
      contentElement.style.top = (windowHeight - contentHeight - 4) + 'px';
    } else {
      contentElement.style.position = 'static';
    }
  }

  if (Settings.autoscroll)
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

/// http://www.websanova.com/tutorials/jquery/jquery-remove-class-by-regular-expression
(function ($) {
  $.fn.removeClassRegEx = function (regex) {
    var classes = $(this).attr('class');

    if (!classes || !regex) return false;

    var classArray = [];
    classes = classes.split(' ');

    for (var i = 0, len = classes.length; i < len; i++)
      if (!classes[i].match(regex)) classArray.push(classes[i]);

    $(this).attr('class', classArray.join(' '));
  };
})(jQuery);


if (typeof SimpleChat === 'undefined') {
  SimpleChat = {
    bytesToHuman: function(size)           { return '0 bytes'; },
    channel: function(id)                  { return '{}'; },
    day: function(day)                     { return day; },
    encryption: function()                 { return false; },
    feed: function(id, name, options)      { return false; },
    get: function(id, name, json)          {},
    id: function()                         { return ''; },
    isOnline: function()                   { return true; },
    match: function(channelId, userId)     { return -1; },
    mdate: function(id, name)              { return 0; },
    randomId: function()                   { return ''; },
    request: function(id, cmd, name, json) {},
    serverId: function()                   { return ''; },
    serverPeer: function()                 { return ''; },
    setTabPage: function(id, page)         {},
    status: function(id)                   { return 'Offline'; },
    statusText: function(status)           { return ''; },
    toBase32: function(text)               { return ''; },
    traffic: function()                    { return {tx:0, rx:0}; },
    translate: function(key)               { return key; },
    value: function(key)                   { return null; },
    pinTab: function(id)                   {},
    aclToInt: function(acl)                { return 6; }
  };
}
else {
  SimpleChat.renamed.connect(Messages.rename);
  SimpleChat.recolored.connect(Messages.recolor);
  SimpleChat.settingsChanged.connect(Settings.changed);
}

if (typeof ChatView === 'undefined') {
  ChatView = {
    jsfiles: function()            { return []; },
    loadFinished: function()       {},
    getId: function()              { return ''; },
    setLastMessage: function(date) {},
    removeDay: function(day)       {},
    allowFocus: function(allow)    {}
  }
}
else {
  ChatView.reload.connect(Messages.reload);
  ChatView.messages.connect(Messages.addMessages);
  ChatView.feed.connect(Messages.feed);
}
