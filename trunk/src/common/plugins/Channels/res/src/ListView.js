
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

    ChannelsList.get();
  },


  /*
   * Загрузка списка каналов.
   */
  get: function() {
    SimpleChat.feed(SimpleChat.serverId(), 'list', 1);
    Loader.spinner.add('loading/channels');
  }
};


/*
 * Чтение данных фидов.
 */
ChannelsList.feed = {
  /*
   * Чтение новых данных.
   */
  read: function(json) {
    if (json.type == 'body') {
      if (json.feed == 'list')
        ChannelsList.feed.list(json.data, json.status);
    }
  },


  /*
   * Чтение фида "list".
   */
  list: function(json, status) {
    $('.channel-item').remove();

    if (status == 200) {
      var channels     = json.channels;
      var channelsList = $('#channels-list');

      for (var i = 0; i < channels.length; i++) {
        var channel = channels[i];
        if (channel.length < 4)
          continue;

        channelsList.append(ChannelsList.feed.listItem(channel));
      }
    }

    Loader.spinner.remove('loading/channels');
  },


  /*
   * Формирование HTML кода для канала.
   */
  listItem: function(list) {
    var title = list[3];
    if (title == '')
      title = '<span data-tr="channels_no_title">' + Utils.tr('channels_no_title') + '</span>';

    return '<div class="channel-item">' +
             '<div class="channel-item-header">' +
               '<a href="#" class="nick color-default" id="' + list[0] + '">' + list[1] + '</a>' +
               '<span class="badge">' + list[2] + '</span>' +
             '</div>' +
             '<div class="channel-item-title">' + title + '</div>' +
           '</div>'
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


  /*
   * Обработка нажатия на ссылку входа в канал по его идентификатору.
   */
  $('body').on('click.channel', '.nick', function (event) {
    event.preventDefault();
    ChannelsView.join($(this).attr('id'));
  });

  /*
   * Обновление списка каналов.
   */
  $('#reload').on('click.reload', function(event) {
    event.preventDefault();
    ChannelsList.online();
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
else {
  ChannelsView.feed.connect(ChannelsList.feed.read);
}
