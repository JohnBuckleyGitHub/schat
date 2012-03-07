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

  var feed = SimpleChat.feed("hosts", false);
  if (feed == "")
    return;

  var json = JSON.parse(feed);
  Server.hosts(json);
};


Pages.feedData = function(data)
{
  var json = JSON.parse(data);
  if (json.own === true && json.name == "hosts")
    Server.hosts(json.feed);
};


var Server = {
  hosts: function(json)
  {
    $("#main-spinner").show();
    $(".host-row").hide();

    for (var key in json) if (json.hasOwnProperty(key) && key.length == 34) {
      Server.host(key, json[key]);
    }

    $("#hosts-content p").show();
    $("#hosts-content #fieldset").show();
    $("#main-spinner").hide();
    $(".host-row:hidden").remove();
    $(".tooltip").easyTooltip();
  },


  host: function(key, json)
  {
    var id = "#" + key;
    if (!$(id).length) {
      var out = '<tr class="host-row" id="' + key + '"><td class="os-cell"><i class="icon-os"></i></td><td class="host-name"></td>' +
        '<td><i class="icon-info tooltip"></i></td><td><a class="btn btn-small">Unlink</a></td></tr>';
      $("#account-table > tbody").append(out);
    }
    else
      $(id).show();

    $(id + " > .host-name").text(json.name);
    $(id + " .icon-os").attr("class", "icon-os os-" + Pages.os(json.os));
    $(id + " .icon-info").attr("title", Utils.makeTable({'<b>Version:</b>':json.version, '<b>Last IP Address:</b>': json.host}));
  }
};
