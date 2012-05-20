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

var YouTube = {
  onAdd: function(id) {
    var youtube = $('#' + id + ' .youtube');
    if (!youtube.length)
      return;

    youtube.each(function() {
      var vid = $(this).attr('data-youtube-v');
      $(this).after(' <i data-youtube-v="' + vid + '" class="icon-spinner-small"></i>');

      $.ajax({
        url: 'http://gdata.youtube.com/feeds/api/videos/' + vid + '?alt=json',
        dataType: 'json',
        success: function(data) {
          YouTube.setTitle(data, vid);
        }
      });
    });
  },

  setTitle: function(data, vid) {
    $('.icon-spinner-small[data-youtube-v="' + vid + '"]').remove();

    var title = data.entry["title"].$t;
    $('.youtube[data-youtube-v="' + vid + '"]').text(title);

    alignChat();
  }
};

Messages.onAdd.push(YouTube.onAdd);
