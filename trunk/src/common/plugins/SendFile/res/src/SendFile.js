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

Messages.addFileMessage = function(json)
{
  var id = json.Id;
  var html = '<div class="container ' + json.Type + '-type" id="' + json.Id + '">';
  html += '<div class="blocks ' + json.Direction + '">';
  html += '<div class="file-sender">' + DateTime.template(json.Date, json.Day) + Messages.nameBlock(json.Author) + '</div>';
  html += '<div class="file-block"><span class="file-name">' + json.File + '</span><br><span class="file-state">&nbsp;</span></div>';
  html += '<div class="file-buttons btn-group"><a class="btn btn-small btn-file-cancel" data-tr="file-cancel" href="sendfile:void">' + Utils.tr('file-cancel') + '</a></div>';
  html += '<div class="file-progress"><div class="bar"></div></div><div style="clear:both;"></div>';
  html += '</div></div>';

  Messages.addHintedRawMessage(html, json.Hint, id);

  if (json.Direction === 'outgoing') {
    var state = $('#' + id + ' .file-state');
    state.html(Utils.tr('file-waiting'));
    state.attr('data-tr', 'file-waiting');
  }
};