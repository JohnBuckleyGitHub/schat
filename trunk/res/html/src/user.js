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

  // Чтение фида.
  read: function(json) {
    if (!json.hasOwnProperty("head"))
      return;

    $(".profile-row").remove();

    var fields = SimpleChat.fields();
    for (var i = 0; i < fields.length; i++) {
      var field = fields[i];
      if (json.hasOwnProperty(field)) {
        Profile.Field.generic(field, json[field]);
      }
    }
  },

  reload: function() {
    if (Pages.current != 1)
      return;

    Utils.TR("profile");
    Profile.read(SimpleChat.feed(Settings.id, "profile"));
  },

  // Перевод текстовых строк.
  retranslate: function() {
    Utils.TR("profile");
  },

  Field: {
    generic: function(key, value) {
      if (typeof value !== 'string')
        return;

      if (value === "")
        return;

      Profile.addRow(key, value);
    }
  }
};

Pages.onInfo.push(Profile.reload);

SimpleChat.retranslated.connect(Profile.retranslate);
ChatView.reload.connect(Profile.reload);
