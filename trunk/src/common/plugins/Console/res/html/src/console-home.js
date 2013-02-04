
Console.home = {
  /*
   * Загрузка шаблона страницы.
   */
  load: function() {
    $.ajax({
      url: 'home.html',
      isLocal: true,
      dataType: 'html',
      success: Console.home.loaded
    });
  },


  /*
   * Обработка успешной загрузки страницы.
   */
  loaded: function(data) {
    $('#page').html(data);

    Console.current = 'home';
    Utils.retranslate();

    var channel = SimpleChat.channel(SimpleChat.serverId());
    if (channel !== null)
      Console.setName(channel.Name);

    $('#nav-logout').on('click.home', function(event) {
      event.preventDefault();

      Loader.spinner.add('loading/logout');
      SimpleChat.request(SimpleChat.serverId(), 'delete', 'console/me');
    });

    var body  = $('body');

    /*
     * Установка имени сервера.
     */
    body.on('click.name', '#name-ok', function (event) {
      event.preventDefault();
      ConsoleView.setName(SimpleChat.serverId(), $('#name-edit').val());
      $('#modal').modal('hide');
    });


    Utils.adjustWidth($('#about-server-block .field-row-label'));
    Utils.adjustWidth($('#users-online-block .field-row-label'));
    Console.home.online();
  },


  /*
   * Успешное подключение к серверу.
   */
  online: function() {
    Console.home.getFeed(FEED_NAME_SERVER);
    Console.home.getFeed(FEED_NAME_USERS);

    var data = SimpleChat.encryption();
    var encryption = $('#server-encryption');

    if (data !== null) {
      encryption.removeAttr('data-tr');
      encryption.removeClass('red-text');

      var protocol = '';
      if (data.protocol == 2)
        protocol = 'TLS v1';
      else if (data.protocol == 0)
        protocol = 'SSL v3';

      encryption.html('<span class="green-text">' + protocol + '</span>');
      var days = data.daysToExpiry;

      if (days > 0 && days <= 42)
        encryption.append(' <span class="orange-text">' + ConsoleView.expirationText(days) + '</span>');

      if (days <= 0)
        encryption.append(' <span class="red-text" data-tr="console_cert_expired">' + Utils.tr('console_cert_expired') + '</span>');
    }
    else {
      encryption.addClass('red-text');
      encryption.attr('data-tr', 'no-encryption');
      encryption.text(Utils.tr('no-encryption'));
    }
  },


  /*
   * Загрузка фида.
   * Функция пытается использовать кэшированную версию фида и создаёт запрос к серверу для проверки изменился фид или нет.
   *
   * \param name Имя фида.
   */
  getFeed: function(name) {
    Console.feed[name].body(SimpleChat.feed(SimpleChat.serverId(), name, 3), SimpleChat.serverId(), 300);

    SimpleChat.feed(SimpleChat.serverId(), name, 1);
  },


  prepare: function(name, status) {
    if (status == 300) {
      Loader.spinner.add('loading/' + name);
      return true;
    }

    Loader.spinner.remove('loading/' + name);
    return status == 200;
  }
};


/*
 * Чтение ответа на "delete" запрос "console/me".
 */
Console.feed.console.del.me = function(json) {
  if (json.status == 200) {
    Loader.spinner.add('loading/try');
    SimpleChat.get(SimpleChat.serverId(), 'console/try');
  }

  Loader.spinner.remove('loading/logout');
};


/*
 * Обработка получения тела фида FEED_NAME_SERVER.
 */
Console.feed.server.body = function(json, id, status) {
  if (!Console.home.prepare(FEED_NAME_SERVER, status) || json === false)
    return;

  var version = $('#server-version');
  version.text(json.version);
  version.append(' <i class="icon-os os-' + Pages.os(json.os) + '"></i>');
};


/*
 * Обработка получения тела фида FEED_NAME_USERS.
 */
Console.feed.users.body = function(json, id, status) {
  if (SimpleChat.serverId() != id || !Console.home.prepare(FEED_NAME_USERS, status) || json === false)
    return;

  var now  = $('#users-online-now');
  var peak = $('#users-online-peak');

  now.text(json.count);
  peak.text(json.peak.count);
  peak.append(' ' + DateTime.template(json.peak.date, true));
};


Modal.create.name = function(event)
{
  var h3 = $('#modal-header h3');
  h3.text(Utils.tr('console_server_name'));
  h3.attr('data-tr', 'console_server_name');

  $('#modal-body').append(
    '<form id="title-form">' +
      '<div id="name-group" class="control-group">' +
        '<input id="name-edit" type="text" maxlength="20" class="input-append">' +
        '<button id="name-ok" type="submit" class="btn btn-prepend" data-tr="ok">' + Utils.tr('ok') + '</button>' +
      '</div>' +
    '</form>'
  );

  var channel = SimpleChat.channel(SimpleChat.serverId());
  if (channel !== null)
    $('#name-edit').val(channel.Name);
};


Modal.shown.name = function()
{
  $('#name-edit').focus();
};


if (typeof SimpleChat !== 'undefined') {
  SimpleChat.online.connect(Console.home.online);
}
