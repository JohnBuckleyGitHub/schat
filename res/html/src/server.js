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
    if (Pages.current != 1)
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
      var out = '<tr class="host-row" id="' + key + '"><td class="os-cell"><i class="icon-status-offline host-status"></i><a class="host-os" rel="tooltip"><i class="icon-os"></i></a></td><td class="host-name"></td>' +
        '<td><a class="host-info" rel="tooltip"><i class="icon-info"></i></a> <span class="last-activity"></span></td><td><a onclick="" class="btn btn-small btn-unlink" data-tr="unlink">Unlink</a></td></tr>';
      $("#account-table > tbody").append(out);
    }
    else
      $(id).show();

    $(id + " > .host-name").text(json.name);
    $(id + " .icon-os").attr("class", "icon-os os-" + Pages.os(json.os));
    $(id + " .host-os").attr("data-original-title", htmlspecialchars(json.osName));
    $(id + " .host-info").attr("data-original-title", Utils.table({'version': json.version, 'last_ip': json.host}));
    $(id + " .host-status").attr("class", (json.online === true ? "icon-status" : "icon-status-offline") + " host-status");
    $(id + " .last-activity").html(DateTime.template(json.date, true));

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
    $('a[rel=tooltip]').tooltip();

    $("#main-spinner").hide();
    Utils.retranslate();
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

try {
  ChatView.feed.connect(Hosts.feed);
  ChatView.reload.connect(Hosts.reload);
} catch (e) {}


/*
 * Авторизационный диалог.
 */
var AuthDialog = {
  /*
   * Показ диалога.
   */
  show: function()
  {
    Modal.current = 'auth';
    Modal.create.auth(null);

    $('#modal').modal();
  },

  /*
   * Формирование кнопок с провайдерами.
   */
  providers: function()
  {
    var providers = Auth.providers();
    if (!providers.hasOwnProperty('order'))
      return;

    for (var i = 0; i < providers.order.length; i++) {
      var provider = providers.order[i];
      if (providers.providers.hasOwnProperty(provider))
        AuthDialog.addProvider(provider, providers.providers[provider]);
    }

    $('#modal-header h3 .icon-spinner').remove();
    Utils.adjustWidth($('#providers .btn'));
  },

  /*
   * Добавление одиночного провайдера.
   */
  addProvider: function(name, data) {
    $('#providers').append('<a class="btn" href="' + data.url + ' "><i class="provider-' + name + '"></i> ' + data.htmlName + '</a>');
  },

  /*
   * Скрытие диалога.
   */
  hide: function()
  {
    $('#modal').modal('hide');
  }
};


Modal.create.auth = function(event)
{
  $('#modal-header h3 *').remove();
  $('#modal-body *').remove();

  $('#modal-header h3').append('<span></span>');
  $('#modal-header h3').append(' <i class="icon-spinner"></i>');
  $('#modal-header h3 span').text(Utils.tr('sign_in_with'));
  $('#modal-header h3 span').attr('data-tr', 'sign_in_with');

  $('#modal-body').append('<div id="providers"></div>');
  
  if (Auth.anonymous()) {
    $('#modal-body').append('<div id="anonymous-auth"><a data-tr="or_anon_connect" href="#">' + Utils.tr('or_anon_connect') + '</a></div>');

    $("#anonymous-auth a").on("click", function(event) {
      event.preventDefault();
      Auth.open();
    });
  }

  AuthDialog.providers();
};


Modal.shown.auth = function()
{
  Utils.adjustWidth($('#providers .btn'));
};


Modal.hidden.auth = function()
{
  Auth.cancel();
};


if (typeof Auth === "undefined") {
  Auth = {
    providers: function() { return {}; },
    cancel: function() {},
    anonymous: function() { return true; },
    open: function() {}
  }
}
else {
  Auth.providersReady.connect(AuthDialog.providers);
}