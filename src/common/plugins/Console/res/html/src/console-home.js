
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
      $('#server-name').html(channel.Name);

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
  }
};


/*
 * Чтение ответа на "delete" запрос "console/me".
 */
Console.feed.console.del.me = function(json) {
  console.log(json);

  if (json.status == 200) {
    Loader.spinner.add('loading/try');
    SimpleChat.get(SimpleChat.serverId(), 'console/try');
  }

  Loader.spinner.remove('loading/logout');
};


Modal.create.name = function(event)
{
  var h3 = $('#modal-header h3');
  h3.text(Utils.tr('console_server_name'));
  h3.attr('data-tr', 'console_server_name');

  $('#modal-body').append(
    '<form id="title-form">' +
      '<div id="name-group" class="control-group">' +
        '<input id="name-edit" type="text" maxlength="20">' +
        '<button id="name-ok" type="submit" class="btn" data-tr="ok">' + Utils.tr('ok') + '</button>' +
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
