
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
