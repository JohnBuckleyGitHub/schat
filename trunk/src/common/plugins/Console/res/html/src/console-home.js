
Console.home = {
  /*
   * Загрузка шаблона страницы.
   */
  load: function() {
    $.ajax({
      url: 'home.html',
      isLocal: true,
      dataType: 'html',
      success: this.loaded
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
    if (channel !== null) {
      $('#server-name').html(channel.Name);
      console.log(channel);
    }
//      $('#server-name').html(Messages.nameTemplate(channel));
  }
};
