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

$(document).ready(function() {
  $('body').on('click.history', '#history a', History.click);

  History.loading(Settings.id);
});


var History = {
  date: 0,     /// Дата самого старого полученного сообщения.
  message: '', /// Идентификатор самого старого полученного сообщения.

  /*
   * Показ виджета истории.
   */
  show: function() {
    if (!$('#history').length)
      $('#Chat').prepend('<div id="history-bar" class="history-bar"><div class="history-bar-inner"><div id="history"></div></div></div>');

    $('.history-bar').show();
    alignChat();
  },


  /*
   * Сокрытие виджета истории.
   */
  hide: function() {
    $('.history-bar').hide();
    alignChat();
  },


  /*
   * Обработка клика для загрузки сообщений.
   */
  click: function(event) {
    event.preventDefault();
    if (!History.date)
      return;

    History.loading(Settings.id);
    SimpleChat.get(Settings.id, 'messages/last', { before: History.date });
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
  },


  /*
   * Обработка получения данных фидов.
   */
  feed: function(json) {
    if (json.id != Settings.getId())
      return;

    if (json.type == 'reply' && json.cmd == 'get' && json.name == 'messages/last') {
      History.last(json);
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
      if ($('#' + id).length) {
        History.date = $('#' + id).attr('data-time');
        History.done();
      }
      else
        History.message = id;
    }
    else
      History.hide();
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
  }
};


if (typeof HistoryView === "undefined") {
  HistoryView = {}
}
else {
  HistoryView.loading.connect(History.loading);
  ChatView.feed.connect(History.feed);
}

Messages.onAdd.push(History.onAdd);
