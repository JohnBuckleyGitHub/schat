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

var Channels = {
  /*
   * Обновление информации.
   */
  reload: function() {
    if (Pages.current != 1)
      return;

    if (SimpleChat.isOnline())
      Loader.spinner.add('loading/info');

    Channels.info(SimpleChat.feed(Settings.getId(), 'info', 4), 300);
  },


  /*
   * Чтение фида "info".
   */
  info: function(json, status) {
    if (json.hasOwnProperty('title'))
      $('#channel-title-text').text(json.title.text);

    if (status != 300)
      Loader.spinner.remove('loading/info');
  },


  /*
   * Чтение данных фидов.
   */
  feed: function(json) {
    if (!json.hasOwnProperty('type') || json.id !== Settings.getId())
      return;

    if (json.feed == 'info')
      Channels.info(json.data, json.status);
  }
};

$(document).ready(function() {
  $('#page-header').append('<div id="channel-title"><div id="channel-title-text"></div></div>')
});

Pages.onInfo.push(Channels.reload);

if (typeof ChatView !== 'undefined') {
  ChatView.reload.connect(Channels.reload);
  ChatView.feed.connect(Channels.feed);
}
