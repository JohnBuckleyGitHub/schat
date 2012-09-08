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
    state.show();
  },

  setStateText: function(id, text)
  {
    var state = $('#' + id + ' .file-state');
    state.html(text);
    state.removeAttr('data-tr');
    state.show();
  },


  progress: function(id, text, percent)
  {
    SendFileUtils.setStateText(id, text);
    $('#' + id + ' .file-progress .bar').css('width', percent + '%');
  },


  /*
   * Установка иконки файла.
   *
   * \param id Идентификатор передачи файла.
   */
  setFileIcon: function(id)
  {
    var imageId = $('#' + id).data('imageId');
    try {
      SendFile.fileIcon(id).assignToHTMLImageElement(document.getElementById(imageId));
      $('#' + imageId).show();
    }
    catch (e) {
      $('#' + imageId).hide();
    }
  },


  /*
   * Обновление информации о состоянии передачи файла.
   *
   * \param id Идентификатор передачи файла.
   */
  updateState: function(id)
  {
    if (!$('#' + id).length)
      return;

    var role = SendFile.role(id);
    var state = SendFile.state(id);

    if (state != 'U') {
      $('#' + id + ' .file-name').removeClass('file-only-name');
      $('#' + id + ' .btn-small').remove();
      $('#' + id + ' .file-progress').hide();
    }

    // Ожидание действия пользователя.
    if (state == 'W') {
      if (role) {
        SendFileUtils.setStateText(id, SimpleChat.bytesToHuman(SendFile.size(id)));
        $('#' + id + ' .file-buttons').append(SendFileUtils.button('saveas', id));
      }
      else
        SendFileUtils.setStateTr(id, 'file-waiting');
    }
    // Обработка отмены передачи файла.
    else if (state == 'c') {
      SendFileUtils.setStateTr(id, 'file-cancelled');
    }
    // Подключение.
    else if (state == 'C') {
      SendFileUtils.setStateTr(id, 'file-connecting');
      $('#' + id + ' .file-name').text(SendFile.fileName(id));
    }
    // Передача файла.
    else if (state == 'T') {
      $('#' + id + ' .file-progress').show();
      var progress = SendFile.progressInfo(id);
      if (progress.hasOwnProperty('text'))
        SendFileUtils.progress(id, progress.text, progress.percent);
    }
    // Завершение передачи файла.
    else if (state == 'F') {
      if (role) {
        var urls = SendFile.fileUrls(id);
        SendFileUtils.setStateText(id, '<span data-tr="file-received">' + Utils.tr('file-received') + '</span> ' +
              '<a href="' + urls.dir + '" data-tr="file-show">' + Utils.tr('file-show') + '</a>');

        $('#' + id + ' .file-buttons').append(SendFileUtils.button('open', id));
        $('#' + id + ' .btn-file-open').attr('href', urls.file);
      }
      else
        SendFileUtils.setStateTr(id, 'file-sent');
    }

    if (state == 'W' || state == 'C' || state == 'T')
      $('#' + id + ' .file-buttons').append(SendFileUtils.button('cancel', id));

    alignChat();
  }
};


/*
 * Добавление сообщения с информацией о входящем или отправленном файле.
 */
Messages.addFileMessage = function(json)
{
  var id = json.Id;
  var imageId = SimpleChat.randomId();

  var html = '<div class="container ' + json.Type + '-type" id="' + json.Id + '">';
  html += '<div class="blocks ' + json.Direction + '">';
  html += '<div class="file-sender">' + DateTime.template(json.Date, json.Day) + Messages.nameBlock(json.Author) + '</div>';
  html += '<div class="file-icon"><img id="' + imageId + '" src="" width="16" height="16" alt="" /></div>';
  html += '<div class="file-block"><span class="file-name file-only-name">' + json.File + '</span><br><span class="file-state">&nbsp;</span></div>';
  html += '<div class="file-buttons btn-group"></div>';
  html += '<div class="file-progress"><div class="bar"></div></div><div style="clear:both;"></div>';
  html += '</div></div>';

  Messages.addHintedRawMessage(html, json.Hint, id);
  $('#' + id).data('imageId', imageId);
  SendFileUtils.setFileIcon(id);
};


/*
 * Обновление идентификатора исходящей передачи файла.
 */
Messages.upgradeFileMessage = function(json) {
  if (!$('#' + json.InternalId).length)
    return;

  $('#' + json.InternalId).attr('id', json.Id);
  $('#' + json.Id + ' .btn-file-cancel').attr('href', 'chat-sendfile:cancel/' + json.Id);
};


if (typeof SendFile === "undefined") {
  SendFile = {
    state: function(id) { return 'U'; },
    role: function(id) { return 1; },
    size: function(id) { return 0; },
    fileName: function(id) { return ''; },
    progressInfo: function(id) { return {}; },
    fileUrls: function(id) { return {}; }
  };
}
else {
  SendFile.progress.connect(SendFileUtils.progress);
  SendFile.stateChanged.connect(SendFileUtils.updateState);
}

Messages.onAdd.push(SendFileUtils.updateState);
