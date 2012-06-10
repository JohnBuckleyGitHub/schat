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

var SendFileUtils = {
  button: function(name, id)
  {
    return '<a class="btn btn-small btn-file-' + name + '" data-tr="file-' + name + '" href="chat-sendfile:'
       + name + '/' + id + '">' + Utils.tr('file-' + name) + '</a>';
  },

  setStateTr: function(id, text)
  {
    var state = $('#' + id + ' .file-state');
    state.html(Utils.tr(text));
    state.attr('data-tr', text);
  },

  setStateText: function(id, text)
  {
    var state = $('#' + id + ' .file-state');
    state.html(text);
    state.removeAttr('data-tr');
  },

  // Обработка отмены передачи файла.
  cancelled: function(id)
  {
    SendFileUtils.setStateTr(id, 'file-cancelled');
    $('#' + id + ' .file-buttons').remove();
    $('#' + id + ' .file-progress').remove();
  },

  accepted: function(id, fileName)
  {
    SendFileUtils.setStateTr(id, 'file-connecting');
    $('#' + id + ' .file-name').text(fileName);
    $('#' + id + ' .btn-file-saveas').remove();
  },

  progress: function(id, text, percent)
  {
    SendFileUtils.setStateText(id, text);
    $('#' + id + ' .file-progress .bar').css('width', percent + '%');
  },

  sent: function(id)
  {
    SendFileUtils.setStateTr(id, 'file-sent');
    $('#' + id + ' .file-buttons').remove();
    $('#' + id + ' .file-progress').remove();
  }
};


Messages.addFileMessage = function(json)
{
  var id = json.Id;
  var html = '<div class="container ' + json.Type + '-type" id="' + json.Id + '">';
  html += '<div class="blocks ' + json.Direction + '">';
  html += '<div class="file-sender">' + DateTime.template(json.Date, json.Day) + Messages.nameBlock(json.Author) + '</div>';
  html += '<div class="file-block"><span class="file-name">' + json.File + '</span><br><span class="file-state">&nbsp;</span></div>';
  html += '<div class="file-buttons btn-group">' + SendFileUtils.button('cancel', id) + '</div>';
  html += '<div class="file-progress"><div class="bar"></div></div><div style="clear:both;"></div>';
  html += '</div></div>';

  Messages.addHintedRawMessage(html, json.Hint, id);

  if (json.Direction === 'outgoing') {
    SendFileUtils.setStateTr(id, 'file-waiting');
  }
  else {
    SendFileUtils.setStateText(id, SimpleChat.bytesToHuman(json.Size));
    $('#' + id + ' .file-buttons').prepend(SendFileUtils.button('saveas', id));
  }
};


if (typeof SendFile === "undefined") {
  SendFile = {

  };
}
else {
  SendFile.cancelled.connect(SendFileUtils.cancelled);
  SendFile.accepted.connect(SendFileUtils.accepted);
  SendFile.progress.connect(SendFileUtils.progress);
  SendFile.sent.connect(SendFileUtils.sent);
}