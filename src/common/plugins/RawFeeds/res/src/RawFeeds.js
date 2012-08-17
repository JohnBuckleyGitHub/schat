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

RawFeeds = {
  headers: function(json) {
    var html = '<div class="feeds-headers">';
    for (var key in json) if (json.hasOwnProperty(key)) {
      html += RawFeeds.feed(key, json[key]);
    }

    html += '</div>';
    return html;
  },
  
  feed: function(name, data) {
    return '<div class="feed-header">' + RawFeeds.icon(data.cached) + DateTime.template(data.date, true) + ' <a href="#" class="feed-name">' + name + '</a></div>';
  },

  raw: function(json) {
    var id = json.Id;
    $('#' + id + ' .raw-feeds-title').append(' <a class="btn btn-toggle" id=""><i class="icon-plus-small"></i></a>');
    $.JSONView(json.Text, $('#' + id + " .jsonoutput"));

    $('#' + id + ' .btn-toggle').on('click', function(event) {
      var c = $(this).children();
      if (c.attr('class') == 'icon-plus-small') {
        $('#' + id + ' .jsonoutput').show();
        c.attr('class', 'icon-minus-small');
      }
      else {
        $('#' + id + ' .jsonoutput').hide();
        c.attr('class', 'icon-plus-small');
      }

      alignChat();
    });
  },

  icon: function(cached) {
    return '<i class="icon-feed' + (cached ? '-cached' : '') + '"></i> ';
  }
};


Messages.addRawFeedsMessage = function(json)
{
  var html = '<div class="container ' + json.Type + '-type" id="' + json.Id + '">';
  html += '<div class="blocks ';
  if (json.Extra !== undefined)
    html += json.Extra;

  html += '"><div class="alert ' + (json.Status.Code == 200 ? 'alert-info' : 'alert-error') +'">' +
          '<a class="close" data-dismiss="alert" href="#">×</a>';

  html += '<div class="raw-feeds-header">';
  html += '<h3 class="raw-feeds-title">' + json.Title + '</h3>';
  if (json.Status.Code != 200)
    html += ' <span class="raw-feeds-bad-status">' + json.Status.Code + ' ' + json.Status.Desc + '</span>';

  html += '</div>';

  if (json.Command == 'headers') {
    html += RawFeeds.headers(json.Data);
  }

  html += '<div class="jsonoutput" style="display:none;"></div>';

  html += '</div>';
  html += '</div>';
  html += '</div>';

  Messages.addRawMessage(html);
  RawFeeds.raw(json);
  alignChat();
};


$(document).ready(function() {
  $('body').on('click', '.feed-name', function(event) {
    event.preventDefault();
    SimpleChat.feed(Settings.id, $(this).text(), false);
  });
});
