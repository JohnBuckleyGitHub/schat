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

var Profile = {
  // Добавление поля профиля.
  addRow: function(key, html) {
    $('#profile-table').append(Utils.row('field-' + key, html));
  },


  body: function(json) {
    Profile.read(json);
  },


  feed: function(json) {
    if (!json.hasOwnProperty('type'))
      return;

    if (json.feed !== 'profile')
      return;

    if (json.id !== Settings.id)
      return;

    try {
      Profile[json.type](json.data);
    }
    catch (e) {}
  },


  /*
   * Чтение фида.
   */
  read: function(json) {
    if (json === false)
      return;

    if (json.hasOwnProperty('provider')) {
      $('#user-account').html('<a href="' + json.link + '"><i class="provider-' + json.provider + '"></i></a>')
    }
    else
      Profile.setAnonymous();

    $("#profile-table *").remove();

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
    Loader.spinner.remove('loading/profile');
  },


  reload: function() {
    if (Pages.current != 1)
      return;

    if (SimpleChat.isOnline())
      Loader.spinner.add('loading/profile');

    Utils.TR('profile');

    Profile.read(SimpleChat.feed(Settings.id, 'profile', 1));
    Profile.setStatus();
  },


  reply: function(json) {
    if (json.action !== "x-set")
      return;

    Profile.read(SimpleChat.feed(Settings.id, 'profile', 3));
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
  body: function(json) {
    Connections.read(json);
  },

  feed: function(json) {
    if (!json.hasOwnProperty('type'))
      return;

    if (json.feed !== 'user')
      return;

    if (json.id !== Settings.id)
      return;

    try {
      Connections[json.type](json.data);
    }
    catch (e) {}
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
  read: function(json) {
    if (json === false || !json.hasOwnProperty('connections'))
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

    Loader.spinner.remove('loading/connections');
  },


  reload: function() {
    if (Pages.current != 1)
      return;

    if (SimpleChat.status(Settings.id) != 'Offline')
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
    Loader.spinner.remove('loading/connections');
  },


  /*
   * Обработка подключения чата.
   */
  online: function()
  {
    if (SimpleChat.isOnline())
      Loader.spinner.add('loading/connections');

    Connections.read(SimpleChat.feed(Settings.id, 'user', 1));
  }
};


var UserHooks = {
  process: [],
  connection: []
};


Modal.create.connection = function(e)
{
  $('#modal-header h3').text(e.target.innerText);

  var id = $(e.target).data('id');

  var feed = SimpleChat.feed(Settings.id, 'user', 3);
  if (feed === false || !feed.hasOwnProperty('connections'))
    return;

  var json = feed.connections[id];

  $('#modal-body').append(Utils.row('chat_version', htmlspecialchars(json.version)));
  $('#modal-body').append(Utils.row('os_name', '<i class="icon-os os-' + Pages.os(json.os) + '"></i> ' + htmlspecialchars(json.osName)));

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

if (typeof ChatView === "undefined") {

}
else {
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
