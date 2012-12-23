
var ChannelsList = {
  /*
   * Динамический перевод интерфейса.
   */
  retranslate: function() {
    $('#join-name').attr('placeholder', Utils.tr('channels_channel_name'))
  },


  /*
   * Успешное подключение к серверу.
   */
  online: function() {
    var channel = SimpleChat.channel(SimpleChat.serverId());
    if (channel !== null)
      $('#server-name').html(channel.Name);
  }
};


$(document).ready(function() {
  Utils.retranslate();
  ChannelsList.retranslate();

  ChannelsList.online();

  /*
   * Обработка нажатия на кнопку входа в канал.
   */
  $('#join').on('click.join', function(event) {
    event.preventDefault();

    var joinName = $('#join-name');
    var name     = joinName.val();

    if (name.length >= 3) {
      ChannelsView.join(name);
      joinName.val('');
    }
  });
});


if (typeof SimpleChat !== 'undefined') {
  SimpleChat.retranslated.connect(ChannelsList.retranslate);
  SimpleChat.online.connect(ChannelsList.online);
}


if (typeof ChannelsView === 'undefined') {
  ChannelsView = {
    join: function(name) {}
  }
}
