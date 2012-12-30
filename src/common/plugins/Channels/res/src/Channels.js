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
  acl: -1, // Права доступа к каналу.

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
    else if (json.feed == 'acl')
      Channels.online();
  },


  /*
   * Успешное подключение к серверу.
   */
  online: function() {
    Channels.acl = SimpleChat.match(Settings.getId(), SimpleChat.id());
    var dropdown = $('#settings-dropdown');

    if (Channels.acl == 7) {
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
             '<li><a href="#" data-tr="channels_title" class="modal-toggle" data-handler="title">' + Utils.tr('channels_title') + '</a></li>' +
             '<li><a href="#" data-tr="channels_options" class="modal-toggle" data-handler="options">' + Utils.tr('channels_options') + '</a></li>' +
           '</ul>';
  }
};


/*
 * Создание модального окна для изменения заголовка канала.
 */
Modal.create.title = function(event)
{
  ChatView.allowFocus(true);

  var h3 = $('#modal-header h3');
  h3.text(event.target.innerText);
  h3.attr('data-tr', 'channels_title');

  $('#modal-body').append(
    '<form id="title-form">' +
     '<div id="title-group" class="control-group">' +
       '<input id="title-edit" type="text" maxlength="200">' +
       '<button id="title-ok" type="submit" class="btn" data-tr="ok">' + Utils.tr('ok') + '</button>' +
     '</div>' +
    '</form>'
  );

  $('#title-edit').val($('#channel-title-text').text());
};


/*
 * Установка фокуса на поле редактирования заголовка канала.
 */
Modal.shown.title = function()
{
  $('#title-edit').focus();
};


/*
 * Удаление фокуса при закрытии окна.
 */
Modal.hidden.title = function()
{
  ChatView.allowFocus(false);
};


/*
 * Изменение опций канала.
 */
Modal.create.options = function(event)
{
  var h3 = $('#modal-header h3');
  h3.text(event.target.innerText);
  h3.attr('data-tr', 'channels_options');

  var feed = SimpleChat.feed(Settings.getId(), 'info', 4);
  if (feed !== false)
    var visibility = feed.visibility || 0;

  var body = $('#modal-body');
  body.append('<form>');
  body.append(
    '<div id="visibility-row" class="row">' +
      '<label for="visibility" data-tr="channels_visibility">' + Utils.tr('channels_visibility') + '</label>' +
      '<select id="visibility">' +
        '<option value="0"  data-tr="channels_default">'                                                   + Utils.tr('channels_default')        + '</option>' +
        '<option value="1"  ' + (visibility > 0 ? 'selected' : '') + ' data-tr="channels_always_visible">' + Utils.tr('channels_always_visible') + '</option>' +
        '<option value="-1" ' + (visibility < 0 ? 'selected' : '') + ' data-tr="channels_hidden">'         + Utils.tr('channels_hidden')         + '</option>' +
      '</select>' +
    '</div>'
  );

  if (SimpleChat.match(SimpleChat.serverId(), SimpleChat.id()) == 7) {
    body.append(
      '<div id="pin-row" class="row">' +
        '<input id="pin" type="checkbox" ' + (feed.pinned == true ? 'checked' : '') + '> ' +
        '<label for="pin" data-tr="channels_pin">' + Utils.tr('channels_pin') + '</label>' +
      '</div>'
    );
  }

  body.append('</form>');
};


$(document).ready(function() {
  $('#page-header').append('<div id="channel-title"><div id="channel-title-text"></div></div>');
  var modal = $('#modal-body');


  /*
   * Установка нового заголовка канала.
   */
  modal.on('click.title', '#title-ok', function (event) {
    event.preventDefault();

    var text = $('#title-edit').val();
    $('#modal').modal('hide');

    if ($('#channel-title-text').text() != text)
      SimpleChat.request(Settings.getId(), 'post', 'info/title', {'value':text, 'options':7});
  });


  /*
   * Обработка изменения видимости канала.
   */
  modal.on('change.visibility', '#visibility', function (event) {
    var value = $(this).find('option:selected').attr('value');
    SimpleChat.request(Settings.getId(), 'post', 'info/visibility', {'value':value, 'options':7});
  });


  /*
   * Обработка изменения закрепления канала.
   */
  modal.on('change.pinned', '#pin', function (event) {
    SimpleChat.request(Settings.getId(), 'post', 'info/pinned', {'value':$(this).is(':checked'), 'options':7});
  });


  Channels.online();
});

Pages.onInfo.push(Channels.reload);

if (typeof ChatView !== 'undefined') {
  ChatView.reload.connect(Channels.reload);
  ChatView.feed.connect(Channels.feed);
}

if (typeof SimpleChat !== 'undefined') {
  SimpleChat.online.connect(Channels.online);
}
