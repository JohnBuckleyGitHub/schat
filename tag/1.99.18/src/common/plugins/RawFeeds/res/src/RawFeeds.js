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

Messages.addRawFeedsMessage = function(json)
{
  var html = '<div class="container ' + json.Type + '-type" id="' + json.Id + '">';
  html += '<div class="blocks ';
  if (json.Extra !== undefined)
    html += json.Extra;

  html += '">';

  html += '<div class="raw-feeds-header">';
  html += DateTime.template(json.Date, false);
  html += '<b class="raw-feeds-command">' + json.Command + '</b>';
  if (json.Status.Code != 200)
    html += ' <span class="raw-feeds-bad-status">' + json.Status.Code + ' ' + json.Status.Desc + '</span>';

  html += '</div>';

  html += '<div class="jsonoutput"></div>';

  html += '</div>';
  html += '</div>';

  Messages.addRawMessage(html);

  if (json.Text.length)
    $.JSONView(json.Text, $('#' + json.Id + " .jsonoutput"));

  alignChat();
};
