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
  read: function(json) {
    if (!json.hasOwnProperty("head"))
      return;

    for (var key in json) if (json.hasOwnProperty(key) && key !== "head") {
      Profile.Field.generic(key, json[key]);
    }
  },

  reload: function() {
    if (Pages.current != 1)
      return;

    Profile.read(SimpleChat.feed(Settings.id, "profile"));
  },

  Field: {
    generic: function(key, json) {
      if (typeof json !== 'string')
        return;

      if (json === "")
        return;

      var out = '<tr><td>' + key + '</td><td>' + json + '</td></tr>';
      $("#profile-table > tbody").append(out);
    }
  }
};

Pages.onInfo.push(Profile.reload);

ChatView.reload.connect(Profile.reload);
