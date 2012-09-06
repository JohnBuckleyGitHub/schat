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
  History.loading(Settings.id);
});


var History = {
  /*
   * Показ виджета истории.
   */
  show: function() {
    if (!$('#history').length)
      $('#Chat').prepend('<div class="history-bar"><div class="history-bar-inner"><div id="history"></div></div></div>');

    $('.history-bar').show();
  },


  /*
   * Уведомление пользователя о загрузке сообщений.
   */
  loading: function(id) {
    if (Settings.id != id)
      return;

    History.show();
    $('#history').html('<i class="icon-spinner-small"></i> <span data-tr="history_loading">' + Utils.tr('history_loading') + '</span>')
  },

  feed: function(json)
  {
    console.log(json);
  }
};


if (typeof HistoryView === "undefined") {
  HistoryView = {}
}
else {
  HistoryView.loading.connect(History.loading);
  ChatView.feed.connect(History.feed);
}