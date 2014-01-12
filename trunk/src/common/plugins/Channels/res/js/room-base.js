/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright (c) 2008-2014 IMPOMEZIA <schat@impomezia.com>
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

var ACL_DEFAULT   = -1;
var ACL_OWNER     = 15; // rwX
var ACL_MODERATOR = 22; // rW-
var ACL_READWRITE = 6;  // rw-
var ACL_READONLY  = 4;  // r--
var ACL_FORBIDDEN = 0;  // ---

var ACL_CHANNEL_READWRITE = 502;
var ACL_CHANNEL_READONLY  = 484;
var ACL_CHANNEL_FORBIDDEN = 448;

var Channels = {
  acl: -1,     // Права доступа к каналу.
  timeout: {
    visibility: null,
    acl: null,
    logging: null
  },

  /*
   * Обновление информации.
   */
  reload: function() {
    if (Pages.current != 1)
      return;

    if (SimpleChat.isOnline())
      Loader.spinner.add('loading/info');

    Channels.info(SimpleChat.feed(Settings.getId(), FEED_NAME_INFO, 4), 300);
  },


  /*
   * Чтение фида FEED_NAME_INFO.
   */
  info: function(json, status) {
    if (json.hasOwnProperty('title'))
      document.getElementById('channel-title-text').textContent = json.title.text;

    if (status != 300)
      Loader.spinner.remove('loading/info');
  },


  /*
   * Чтение данных фидов.
   */
  feed: function(json) {
    if (json === false)
      return;

    if (json.feed == FEED_NAME_INFO && json.id == Settings.getId())
      Channels.info(json.data, json.status);
    else if (json.feed == FEED_NAME_ACL)
      Channels.online();
  },


  /*
   * Успешное подключение к серверу.
   */
  online: function() {
    Channels.acl = SimpleChat.match(Settings.getId(), SimpleChat.id());
    var dropdown = $('#settings-dropdown');

    if (Channels.acl != -1 && (Channels.acl & 9)) {
      if (!dropdown.length)
        $('#channel-buttons').append('<div id="settings-dropdown" class="dropdown pull-right"><a id="channel-settings" data-toggle="dropdown" href="#"></a>' + Channels.menu() + '</div>');
    }
    else
      dropdown.remove();
  },


  /*
   * Формирование меню канала.
   */
  menu: function() {
    return '<ul class="dropdown-menu" role="menu" aria-labelledby="dropdownMenu">' +
             '<li><a href="#" data-tr="channels-title" class="modal-toggle" data-handler="title">' + tr('channels-title') + '</a></li>' +
             '<li><a href="#" data-tr="channels-options" class="modal-toggle" data-handler="options">' + tr('channels-options') + '</a></li>' +
           '</ul>';
  },


  /*
   * Показ окна редактирования расширенных прав доступа.
   */
  editAcl: function(id) {
    $(schat.ui.modal.element).modal('hide');
    schat.ui.modal.current = new schat.ui.RoomAclDialog(id);
    $(schat.ui.modal.element).modal();
  }
};


/**
 * Создание модального окна для изменения заголовка комнаты.
 */
Modal.create.title = function()
{
  schat.ui.modal.current = new schat.ui.RoomTitleDialog();
};


/**
 * Создание модального окна для изменения опций комнаты.
 */
Modal.create.options = function() {
  schat.ui.modal.current = new schat.ui.RoomOptionsDialog();
};


$(document).ready(function() {
  $('#page-header').append('<div id="channel-title"><div id="channel-title-text"></div></div>');

  Channels.online();
});

Pages.onInfo.push(Channels.reload);

try {
  ChatView.reload.connect(Channels.reload);
  ChatView.feed.connect(Channels.feed);
  SimpleChat.online.connect(Channels.online);
}
catch (e) {}
