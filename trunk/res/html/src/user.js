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
    var out = '<tr class="profile-row" id="field-' + key + '">' +
      '<td class="field-label"><span data-tr="field-' + key + '">' + Utils.tr('field-' + key) + '</span>:</td>' +
      '<td class="field-value">' + html + '</td></tr>';

    $("#profile-table > tbody").append(out);
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

    try {
      Profile[json.type](json.data);
    }
    catch (e) {}
  },


  // Чтение фида.
  read: function(json) {
    if (!json.hasOwnProperty("head"))
      return;

    $(".profile-row").remove();

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


  Field: {
    name: function(key, value) {
      Profile.addRow(key, Utils.left(htmlspecialchars(Utils.simplified(value)), 128));
    }
  }
};


var Connections = {
  onProcess: [],

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

    try {
      Connections[json.type](json.data);
    }
    catch (e) {}
  },

  process: function(key, json) {
    var id = "#" + key;
    $("#connections").append('<div class="connection-row bottom-line" id="' + key + '"><i class="icon-os"></i> <span class="connection-host"></span></div>');

    $(id + " .icon-os").attr("class", "icon-os os-" + Pages.os(json.os));
    $(id + " .icon-os").attr("data-original-title", htmlspecialchars(json.osName));
    $(id + " > .connection-host").text(json.host);
  },

  // Чтение фида.
  read: function(json) {
    if (!json.hasOwnProperty("head") && !json.hasOwnProperty("connections"))
      return;

    $(".connection-row").remove();

    var connections = json.connections;
    var count = 0;

    for (var key in connections) if (connections.hasOwnProperty(key) && key.length == 34) {
      count++;
      Connections.process(key, connections[key]);

      for (var i = 0; i < Connections.onProcess.length; i++) {
        Connections.onProcess[i](key, connections[key]);
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

Pages.onInfo.push(Profile.reload);
Pages.onInfo.push(Connections.reload);

try {
  ChatView.feed.connect(Profile.feed);
  ChatView.reload.connect(Profile.reload);

  ChatView.feed.connect(Connections.feed);
  ChatView.reload.connect(Connections.reload);
} catch (e) {}
