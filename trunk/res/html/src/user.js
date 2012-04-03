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
      '<td class="field-label" data-tr="field-' + key + '">' + Utils.tr('field-' + key) + ':</td>' +
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
    Utils.retranslate();
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

Pages.onInfo.push(Profile.reload);
ChatView.feed.connect(Profile.feed);
ChatView.reload.connect(Profile.reload);
