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

Pages.onInfo = function()
{
  $("#info-content > h1").html(Messages.nameTemplate(JSON.parse(SimpleChat.channel(Settings.id))));

  var feed = SimpleChat.feed("hosts");
  if (feed == "")
    return;

  var json = JSON.parse(feed);
  Server.hosts(json);
};


var Server = {
  hosts: function(json)
  {
    $(".host-row").hide();

    for (var key in json) if (json.hasOwnProperty(key) && key.length == 34) {
      Server.host(key, json[key]);
    }
  },


  host: function(key, json)
  {
    var out = '<tr class="host-row" id="' + key + '"><td><i class="icon-unknown"></i></td><td class="host-name"></td><td>0<i class="icon-spinner"></i></td><td>Rename</td><td>Unlink</td></tr>';
    $("#account-table > tbody").append(out);

    var id = "#" + key;
    $(id + " > .host-name").text(json.name);
  }
};