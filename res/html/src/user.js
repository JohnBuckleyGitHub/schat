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
    if (Pages.current != 1)
      return;

    if (!json.hasOwnProperty("type"))
      return;

    if (json.name !== "profile")
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
    if (!json.hasOwnProperty("head"))
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
    $("#main-spinner").hide();
  },


  reload: function() {
    if (Pages.current != 1)
      return;

    $("#main-spinner").css("display", "inline-block");
    Utils.TR("profile");

    Profile.read(SimpleChat.feed(Settings.id, "profile"));
  },


  reply: function(json) {
    if (json.action !== "x-set")
      return;

    Profile.read(SimpleChat.feed(Settings.id, "profile"));
  },


  retranslate: function() {
    Utils.adjustWidth($('#profile-table .field-row-label'));
  },


  Field: {
    name: function(key, value) {
      Profile.addRow(key, Utils.left(htmlspecialchars(Utils.simplified(value)), 128));
    }
  },

  /*!
   * Показ о том, что пользователь анонимный.
   *
   * \param anonymous true если пользователь анонимный.
   */
  setAnonymous: function()
  {
    $('#user-account').html('<span data-tr="anonymous_user">' + Utils.tr('anonymous_user') + '</span>');
  }
};


var Connections = {
  body: function(json) {
    Connections.read(json);
  },

  feed: function(json) {
    if (Pages.current != 1)
      return;

    if (!json.hasOwnProperty("type"))
      return;

    if (json.name !== "user")
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
    if (!json.hasOwnProperty("head") && !json.hasOwnProperty("connections"))
      return;

    $(".connection-row").remove();

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

    $("#connections-spinner").hide();
    if (count > 0) {
      $("#user-offline").hide();
      $(".icon-os").tooltip();
    } else
      $("#user-offline").show();
  },


  reload: function() {
    if (Pages.current != 1)
      return;

    $("#connections-spinner").css("display", "inline-block");
    Utils.TR("connections");
    Utils.TR("user_offline");

    Connections.read(SimpleChat.feed(Settings.id, "user"));
    Utils.retranslate();
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

  var feed = SimpleChat.feed(Settings.id, "user");
  if (!feed.hasOwnProperty("head") && !feed.hasOwnProperty("connections"))
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

  ChatView.feed.connect(Connections.feed);
  ChatView.reload.connect(Connections.reload);
}
