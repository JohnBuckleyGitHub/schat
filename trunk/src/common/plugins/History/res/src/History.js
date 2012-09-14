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
 * Ожидание загрузки всех сообщений.
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

  if (!this.messages.length) {
    History.done();

    if (this.id != '') {
      Settings.scrollTo = this.id;
      document.getElementById(this.id).scrollIntoView();
    }
  }
  else
    History.scroll = this;

  this.remove = function(id) {
    var index = this.messages.indexOf(id);
    if (index != -1) {
      this.messages.splice(index, 1);

      if (!this.messages.length) {
        History.scroll = null;
        History.done();

        if (this.id != '')
          Settings.scrollTo = this.id;
      }
    }
  }
}


var History = {
  date: 0,       /// Дата самого старого полученного сообщения.
  message: null, /// Идентификатор самого старого полученного сообщения.
  scroll: null,  /// Отложенный скролл ожидающий загрузки сообщений.
  top: false,    /// true если полученны все сообщения.

  /*
   * Показ виджета истории.
   */
  show: function() {
    if (!$('#history').length)
      $('#Chat').prepend('<div id="history-bar"><div id="history"></div></div>');

    $('#history-bar').show();
  },


  /*
   * Сокрытие виджета истории.
   */
  hide: function() {
    $('#history-bar').hide();

    alignChat();
    Loader.spinner.remove('history_loading');
  },


  /*
   * Обработка клика для загрузки сообщений.
   */
  click: function(event) {
    event.preventDefault();
    if (!SimpleChat.isOnline())
      return;

    History.loading(Settings.id);
    SimpleChat.get(Settings.id, 'messages/last', { before: History.date, user: true });
  },


  /*
   * Уведомление пользователя о загрузке сообщений.
   */
  loading: function(id) {
    if (Settings.id != id)
      return;

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
    History.message = null;

    if (json.status == 200 && json.data.messages.length) {
      var id = json.data.messages[0];
      if ($('#' + id).length)
        History.date = $('#' + id).attr('data-time');
      else
        History.message = id;

      if (json.data.count < 20)
        History.top = true;

      alignChat();
      new HistoryScroll('last', json.data);
    }
    else
      History.hide();
  },


  /*
   * Обработка ответа на "get" запрос к "messages/since".
   */
  since: function(json) {
    if (json.status == 200 && json.data.messages.length)
      new HistoryScroll('since', json.data);
    else
      History.done();
  },


  /*
   * Показ ссылки для загрузки сообщений.
   */
  done: function() {
    if (!History.top) {
      History.show();
      $('#history').html('<a class="history-more btn btn-small" href="#"><i class="icon-history-more"></i> <span data-tr="history_more">' + Utils.tr('history_more') + '</span></a>');
    }
    else
      $('#history-bar').hide();

    alignChat();
    Loader.spinner.remove('history_loading');
  },


  /*
   * Обработка добавления сообщения.
   */
  onAdd: function(id) {
    if (History.message !== null && History.message == id) {
      History.date    = $('#' + id).attr('data-time');
      History.message = null;
    }

    if (History.scroll instanceof HistoryScroll)
      History.scroll.remove(id);
  },


  /*
   * Очистка страницы.
   */
  reload: function() {
    History.message = null;
    History.date    = 0;

    History.top = false;
    History.done();
    alignChat();
  }
};


if (typeof HistoryView === 'undefined') {
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

  if (SimpleChat.isOnline())
    History.loading(Settings.id);
  else
    History.done();
});
