
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
      ChannelsList.setName(channel.Name);

    ChannelsList.get();
  },


  /*
   * Загрузка списка каналов.
   */
  get: function() {
    SimpleChat.feed(SimpleChat.serverId(), FEED_NAME_LIST, 1);
    Loader.spinner.add('loading/channels');
  },


  /*
   * Обновление имени сервера.
   */
  setName: function(name) {
    $('#server-name').text(name);
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
      if (json.feed == FEED_NAME_LIST)
        ChannelsList.feed.list(json.data, json.status);
    }
  },


  /*
   * Чтение фида FEED_NAME_LIST.
   */
  list: function(json, status) {
    $('.channel-item').remove();

    if (status == 200 || status == 303) {
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
               '<a href="' + ChannelsView.toUrl(list[0], list[1]) + '" class="nick color-default" id="' + list[0] + '">' + list[1] + '</a>' +
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
});


if (typeof SimpleChat !== 'undefined') {
  SimpleChat.retranslated.connect(ChannelsList.retranslate);
  SimpleChat.online.connect(ChannelsList.online);
}


if (typeof ChannelsView === 'undefined') {
  ChannelsView = {
    join: function(name) {},
    toUrl: function(id, name) { return '#'; }
  }
}
else {
  ChannelsView.feed.connect(ChannelsList.feed.read);
  ChannelsView.serverRenamed.connect(ChannelsList.setName);
}
