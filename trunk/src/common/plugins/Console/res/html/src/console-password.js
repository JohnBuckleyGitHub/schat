/*
 * Форма изменения пароля.
 */
Console.password = {

  /*
   * Загрузка шаблона страницы.
   */
  load: function() {
    console.log('load');
    $.ajax({
      url: 'password.html',
      isLocal: true,
      success: Console.password.loaded
    });
  },


  /*
   * Обработка успешной загрузки формы.
   */
  loaded: function(data) {
    $('#page').html(data);

    Console.current = 'password';
    Utils.retranslate();
    Console.password.adjustWidth();

    var newPassword = $('#new-password');
    newPassword.focus();

    $('#save').on('click.password', function(event) {
      event.preventDefault();

      SimpleChat.request(SimpleChat.serverId(), 'put', 'storage/password', {'value':ConsoleView.toPassword($('#new-password').val())});

      Loader.spinner.add('loading/try');
      SimpleChat.get(SimpleChat.serverId(), 'console/try');
    });

    $('#cancel').on('click.password', function(event) {
      event.preventDefault();

      Loader.spinner.add('loading/try');
      SimpleChat.get(SimpleChat.serverId(), 'console/try');
    });
  },


  /*
   * Подгонка ширины элементов.
   */
  adjustWidth: function() {
    Utils.adjustWidth($('.control-label'));
    Utils.adjustWidth($('button'));
  }
};

if (typeof SimpleChat !== 'undefined')
  SimpleChat.retranslated.connect(Console.password.adjustWidth);
