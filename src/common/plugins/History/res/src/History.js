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


/*
 * Автоматическая прокрутка к заданному id когда получены все сообщения.
 */
function HistoryScroll(type, data) {
  this.id       = '';
  this.messages = [];

  if (type == 'since' && data.hasOwnProperty('day'))
    this.id = 'day-' + data.day;
  else if (type == 'last' && data.user === true)
    this.id = 'Chat';

  for (var i = 0; i < data.messages.length; i++) {
    if (document.getElementById(data.messages[i]) === null)
      this.messages.push(data.messages[i]);
  }

  if (this.messages.length)
    History.scroll = this;
  else {
    Loader.spinner.remove('history_loading');

    if (this.id != '')
      document.getElementById(this.id).scrollIntoView();
  }

  this.remove = function(id) {
    var index = this.messages.indexOf(id);
    if (index != -1) {
      this.messages.splice(index, 1);

      if (!this.messages.length) {
        History.scroll = null;
        Loader.spinner.remove('history_loading');

        if (this.id != '')
          Settings.scrollTo = this.id;
      }
    }
  }
}


var History = {
  date: 0,      /// Дата самого старого полученного сообщения.
  message: '',  /// Идентификатор самого старого полученного сообщения.
  scroll: null, /// Отложенный скролл ожидающий загрузки сообщений.
  top: false,   /// true если полученны все сообщения.

  /*
   * Показ виджета истории.
   */
  show: function() {
    if (!$('#history').length)
      $('#Chat').prepend('<div id="history-bar" class="history-bar"><div class="history-bar-inner"><div id="history"></div></div></div>');

    $('.history-bar').show();
  },


  /*
   * Сокрытие виджета истории.
   */
  hide: function() {
    $('.history-bar').hide();
  },


  /*
   * Обработка клика для загрузки сообщений.
   */
  click: function(event) {
    event.preventDefault();
    History.loading(Settings.id);
    SimpleChat.get(Settings.id, 'messages/last', { before: History.date, user: true });
  },


  /*
   * Уведомление пользователя о загрузке сообщений.
   */
  loading: function(id) {
    if (Settings.id != id)
      return;

    History.show();
    $('#history').html('<i class="icon-spinner-small"></i> <span data-tr="history_loading">' + Utils.tr('history_loading') + '</span>');
    alignChat();

    Loader.spinner.add('history_loading');
  },


  /*
   * Обработка получения данных фидов.
   */
  feed: function(json) {
    if (json.id != Settings.getId())
      return;

    if (json.type == 'reply' && json.cmd == 'get') {
      if (json.name == 'messages/last')
        History.last(json);
      else if (json.name == 'messages/since')
        History.since(json);
    }
  },


  /*
   * Обработка ответа на "get" запрос к "messages/last".
   */
  last: function(json) {
    History.date    = 0;
    History.message = '';

    if (json.status == 200 && json.data.messages.length) {
      var id = json.data.messages[0];
      if ($('#' + id).length)
        History.date = $('#' + id).attr('data-time');
      else
        History.message = id;

      if (json.data.count < 20) {
        History.hide();
        History.top = true;
      }
      else
        History.done();

      alignChat();
      new HistoryScroll('last', json.data);
    }
    else {
      History.hide();
      alignChat();
      Loader.spinner.remove('history_loading');
    }
  },


  /*
   * Обработка ответа на "get" запрос к "messages/since".
   */
  since: function(json) {
    if (History.top)
      History.hide();
    else
      History.done();

    new HistoryScroll('since', json.data);
  },


  /*
   * Показ ссылки для загрузки сообщений.
   */
  done: function() {
    $('#history').html('<a class="history-more" href="#" data-tr="history_more">' + Utils.tr('history_more') + '</a>');
  },


  /*
   * Обработка добавления сообщения.
   */
  onAdd: function(id) {
    if (History.message != '' && History.message == id) {
      History.date    = $('#' + id).attr('data-time');
      History.message = '';

      if (History.date)
        History.done();
    }

    if (History.scroll instanceof HistoryScroll)
      History.scroll.remove(id);
  },


  /*
   * Очистка страницы.
   */
  reload: function() {
    History.message = '';
    History.date    = 0;

    History.top = false;
    History.done();
    History.show();
    alignChat();
  }
};


if (typeof HistoryView === "undefined") {
  HistoryView = {}
}
else {
  HistoryView.loading.connect(History.loading);
  ChatView.feed.connect(History.feed);
  ChatView.reload.connect(History.reload);
}

Messages.onAdd.push(History.onAdd);

$(document).ready(function() {
  $('body').on('click.history', '#history a', History.click);

  History.loading(Settings.id);
});