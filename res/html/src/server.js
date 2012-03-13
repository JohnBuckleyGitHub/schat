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

// Объект связанный с обработкой таблицы компьютеров.
var Hosts = {
  // Обработка получения тела фида hosts.
  body: function(json)
  {
    Hosts.read(json);
  },


  // Базовый слот обрабатывающий новые данные фидов.
  feed: function(json)
  {
    if (Pages.current == 0)
      return;

    if (!json.hasOwnProperty("type"))
      return;

    if (json.own !== true)
      return;

    if (json.name !== "hosts")
      return;

    try {
      Hosts[json.type](json.data);
    }
    catch (e) {}
  },


  // Обработка одиночного хоста.
  processSingleHost: function(key, json)
  {
    var id = "#" + key;
    if (!$(id).length) {
      var out = '<tr class="host-row" id="' + key + '"><td class="os-cell"><i class="icon-status-offline host-status"></i><i class="icon-os"></i></td><td class="host-name"></td>' +
        '<td><i class="icon-info tooltip"></i> <span class="last-activity"></span></td><td><a onclick="" class="btn btn-small btn-unlink" data-tr="unlink">Unlink</a></td></tr>';
      $("#account-table > tbody").append(out);
    }
    else
      $(id).show();

    $(id + " > .host-name").text(json.name);
    $(id + " .icon-os").attr("class", "icon-os os-" + Pages.os(json.os));
    $(id + " .icon-info").attr("title", Utils.table({'version': json.version, 'last_ip': json.host}));
    $(id + " .host-status").attr("class", (json.online === true ? "icon-status" : "icon-status-offline") + " host-status");

    var unlink = $(id + " .btn-unlink");
    unlink.attr("data-id", key);
    unlink.off("click");
    unlink.on("click", Hosts.unlink);
  },


  // Чтение тела фида hosts.
  read: function(json)
  {
    if (!json.hasOwnProperty("head"))
      return;

    $(".host-row").hide();

    for (var key in json) if (json.hasOwnProperty(key) && key.length == 34) {
      Hosts.processSingleHost(key, json[key]);
    }

    $("#hosts-content p").show();
    $("#hosts-content #fieldset").show();
    $(".host-row:hidden").remove();
    $(".tooltip").easyTooltip();

    if (!Hosts.progress) {
      Hosts.progress = true;
      SimpleChat.request("query", "hosts", {"action":"activity"});
    }

    Hosts.retranslate();
  },


  readActivity: function(key, json)
  {
    $("#" + key + " .last-activity").html(DateTime.template(json.date, true));
  },


  reload: function()
  {
    if (Pages.current != 1)
      return;

    $("#info-content > h1").html(Messages.nameTemplate(JSON.parse(SimpleChat.channel(Settings.id))));
    $("#main-spinner").css("display", "inline-block");
    Utils.TR("my_computers");

    Hosts.read(SimpleChat.feed("hosts", false));
  },


  reply: function(json)
  {
    if (json.action !== "activity")
      return;

    Hosts.progress = false;
    for (var key in json) if (json.hasOwnProperty(key) && key.length == 34) {
      Hosts.readActivity(key, json[key]);
    }

    $("#main-spinner").hide();
  },


  // Перевод текстовых строк.
  retranslate: function()
  {
    Utils.TR("my_computers");
    Utils.TR("my_computers_desc");
    Utils.TR("computer_name");
    Utils.TR("last_activity");
    Utils.TR("actions");
    Utils.TR("unlink");
  },


  unlink: function()
  {
    var query = {};
    query.action = "unlink";
    query.id = $(this).attr("data-id");

    SimpleChat.request("query", "hosts", query);
    return false;
  },

  progress: false
};

Pages.onInfo.push(Hosts.reload);

SimpleChat.retranslated.connect(Hosts.retranslate);
ChatView.feed.connect(Hosts.feed);
ChatView.reload.connect(Hosts.reload);
