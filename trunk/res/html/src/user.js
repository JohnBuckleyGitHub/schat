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

var Profile = {
  // Добавление поля профиля.
  addRow: function(key, html) {
    $('#profile-table').append(Utils.row('field-' + key, html));
  },


  feed: function(json) {
    if (json.type === 'body' && json.feed === FEED_NAME_PROFILE && json.id === Settings.getId())
      Profile.read(json.data, json.status);
  },


  /*
   * Чтение фида.
   */
  read: function(json, status) {
    if (!Loader.spinner.prepare(FEED_NAME_PROFILE, status) || json === false)
      return;

    if (json.hasOwnProperty('provider')) {
      $('#user-account').html('<a href="' + json.link + '"><i class="provider-' + json.provider + '"></i></a>')
    }
    else
      Profile.setAnonymous();

    $('#profile-table *').remove();

    var gender = 'field-gender-' + Profile.gender(SimpleChat.channel(Settings.getId()).Gender);
    Profile.addRow('gender', '<span data-tr="' + gender + '">' + Utils.tr(gender) + '</span>');

    var fields = SimpleChat.fields();
    for (var i = 0; i < fields.length; i++) {
      var field = fields[i];
      if (!json.hasOwnProperty(field))
        continue;

      var value = json[field];
      if (typeof value === "string" && value === "")
        continue;

      try {
        Profile.Field[field](field, value);
      } catch (e) {}
    }

    Profile.retranslate();
  },


  /*
   * Преобразование кода пола в строку.
   */
  gender: function(code) {
    switch (code) {
      case 0:
        return 'male';

      case 100:
        return 'female';

      case 151:
        return 'ghost';

      case 152:
        return 'bot';

      default:
        return 'unknown';
    }
  },


  reload: function() {
    if (Pages.current != 1)
      return;

    Utils.TR('profile');

    Profile.read(SimpleChat.feed(Settings.getId(), FEED_NAME_PROFILE, 3), 302);
    SimpleChat.feed(Settings.getId(), FEED_NAME_PROFILE, 1);
    Profile.setStatus();
  },


  retranslate: function() {
    Utils.adjustWidth($('#profile-table .field-row-label'));
  },


  Field: {
    name: function(key, value) {
      Profile.addRow(key, Utils.left(htmlspecialchars(Utils.simplified(value)), 128));
    }
  },


  /*
   * Показ сообщения о том, что пользователь анонимный.
   */
  setAnonymous: function()
  {
    $('#user-account').html('<span data-tr="anonymous_user">' + Utils.tr('anonymous_user') + '</span>');
  },


  /*
   * Обновление информации о статусе пользователя.
   */
  updateStatus: function(id, status)
  {
    if (Settings.id != id)
      return;

    if (Settings.status == status)
      return;

    Settings.status = status;
    $('#user-status-text').attr('data-tr', 'status_' + status);
    $('#user-status-text').text(Utils.tr('status_' + status));
  },


  /*
   * Обновление информации о статусе пользователя.
   */
  setStatus: function()
  {
    Profile.updateStatus(Settings.id, SimpleChat.status(Settings.id));
  },


  /*
   * Обработка отключения чата.
   */
  offline: function()
  {
    Profile.updateStatus(Settings.id, 'Offline');
  }
};


var Connections = {
  feed: function(json) {
    if (json.type === 'body' && json.feed === FEED_NAME_USER && json.id == Settings.getId())
      Connections.read(json.data, json.status);
  },

  process: function(key, json) {
    var id = "#" + key;
    $("#connections").append('<div class="connection-row bottom-line" id="' + key + '"><i class="icon-os"></i> <a href="#" class="connection-host modal-toggle" data-handler="connection"></a></div>');

    $(id + " .icon-os").attr("class", "icon-os os-" + Pages.os(json.os));
    $(id + " .icon-os").attr("data-original-title", htmlspecialchars(json.osName));
    $(id + " > .connection-host").text(json.host);
    $(id + " > .connection-host").data('id', key);
  },

  /*
   * Чтение фида.
   */
  read: function(json, status) {
    if (!Loader.spinner.prepare(FEED_NAME_USER, status) || json === false)
      return;

    $('.connection-row').remove();

    if (!json.hasOwnProperty('provider'))
      Profile.setAnonymous();

    var connections = json.connections;
    var count = 0;

    for (var key in connections) if (connections.hasOwnProperty(key) && key.length == 34) {
      count++;
      Connections.process(key, connections[key]);

      for (var i = 0; i < UserHooks.process.length; i++) {
        UserHooks.process[i](key, connections[key]);
      }
    }

    if (count > 0) {
      $("#user-offline").hide();
      $(".icon-os").tooltip();
    } else
      Connections.offline();
  },


  reload: function() {
    if (Pages.current != 1)
      return;

    if (SimpleChat.status(Settings.getId()) != 'Offline')
      Connections.online();
    else
      Connections.offline();

    Utils.retranslate();
  },


  /*
   * Обработка отключения чата.
   */
  offline: function()
  {
    Utils.TR("user_offline");
    $("#user-offline").show();
    $(".connection-row").remove();
    Loader.spinner.remove('loading/user');
  },


  /*
   * Обработка подключения чата.
   */
  online: function()
  {
    Connections.read(SimpleChat.feed(Settings.getId(), FEED_NAME_USER, 3), 302);
    SimpleChat.feed(Settings.getId(), FEED_NAME_USER, 1);
  }
};


var UserHooks = {
  process: [],   // Хуки построения списка подключений.
  connection: [] // Хуки отображения модального диалога подключения.
};


Modal.create.connection = function(e)
{
  $('#modal-header h3').text(e.target.innerText);

  var id = $(e.target).data('id');

  var feed = SimpleChat.feed(Settings.id, FEED_NAME_USER, 3);
  if (feed === false || !feed.hasOwnProperty('connections'))
    return;

  var json  = feed.connections[id];
  var modal = $('#modal-body');

  modal.append(Utils.row('chat_version', htmlspecialchars(json.version)));
  modal.append(Utils.row('os_name', '<i class="icon-os os-' + Pages.os(json.os) + '"></i> ' + htmlspecialchars(json.osName)));

  for (var i = 0; i < UserHooks.connection.length; i++) {
    UserHooks.connection[i](json);
  }
};

Modal.shown.connection = function()
{
  Utils.adjustWidth($('#modal-body .field-row-label'));
};


Pages.onInfo.push(Profile.reload);
Pages.onInfo.push(Connections.reload);

if (typeof ChatView !== 'undefined') {
  ChatView.feed.connect(Profile.feed);
  ChatView.reload.connect(Profile.reload);
  SimpleChat.retranslated.connect(Profile.retranslate);
  SimpleChat.statusChanged.connect(Profile.updateStatus);
  SimpleChat.offline.connect(Profile.offline);
  SimpleChat.offline.connect(Connections.offline);
  SimpleChat.online.connect(Connections.online);

  ChatView.feed.connect(Connections.feed);
  ChatView.reload.connect(Connections.reload);
}
