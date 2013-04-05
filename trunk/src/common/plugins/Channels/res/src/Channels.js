/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright (c) 2008-2013 IMPOMEZIA <schat@impomezia.com>
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
    pin: null,
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
      $('#channel-title-text').text(json.title.text);

    if (status != 300)
      Loader.spinner.remove('loading/info');

    Channels.stopSpinner('visibility', status);
    Channels.stopSpinner('pin',        status);
    Channels.stopSpinner('logging',    status);
  },


  /*
   * Завершение показа спиннера если получен фид.
   *
   * \param id     Идентификатор спиннера.
   * \param status Статус ответа на запрос.
   */
  stopSpinner: function(id, status) {
    if (Channels.timeout[id] === null)
      return;

    clearInterval(Channels.timeout[id]);
    $('#' + id + '-spinner').addClass('hide');

    if (status != 200 && status != 303) {
      var error = $('#' + id + '-error');
      error.removeClass('hide');
      error.attr('title', SimpleChat.statusText(status));
    }
  },


  /*
   * Запуск спиннера с задержкой 400 мс.
   */
  startSpinner: function(id) {
    $('#' + id + '-error').addClass('hide');
    Channels.timeout[id] = setTimeout(function() {
      $('#' + id + '-spinner').removeClass('hide');
    }, 400);
  },


  /*
   * Чтение данных фидов.
   */
  feed: function(json) {
    if (json === false)
      return;

    if (json.feed == FEED_NAME_INFO && json.id == Settings.getId())
      Channels.info(json.data, json.status);
    else if (json.feed == FEED_NAME_ACL) {
      if (json.type == 'reply')
        Channels.stopSpinner('acl', json.status);

      Channels.online();
    }
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
             '<li><a href="#" data-tr="channels_title" class="modal-toggle" data-handler="title">' + Utils.tr('channels_title') + '</a></li>' +
             '<li><a href="#" data-tr="channels_options" class="modal-toggle" data-handler="options">' + Utils.tr('channels_options') + '</a></li>' +
           '</ul>';
  },


  /*
   * Показ окна редактирования расширенных прав доступа.
   */
  editAcl: function(id) {
    if (Modal.current !== null)
      return;

    $('#modal-header h3').html(Messages.nameTemplate(SimpleChat.channel(id)));

    var body = $('#modal-body');
    body.append('<form>');
    body.append(
      '<form>' +
        '<div id="acl-row" class="row">' +
          '<label for="acl" data-tr="channels_permissions">' + Utils.tr('channels_permissions') + '</label> ' +
          '<select id="acl" data-user="' + id + '">' +
            '<option value="-1" data-tr="channels_default">'   + Utils.tr('channels_default')   + '</option>' +
            '<option value="15" data-tr="channels_owner">'     + Utils.tr('channels_owner')     + '</option>' +
            '<option value="22" data-tr="channels_moderator">' + Utils.tr('channels_moderator') + '</option>' +
            '<option value="6" data-tr="channels_readwrite">'  + Utils.tr('channels_readwrite') + '</option>' +
            '<option value="4" data-tr="channels_readonly">'   + Utils.tr('channels_readonly')  + '</option>' +
            '<option value="0" data-tr="channels_forbidden">'  + Utils.tr('channels_forbidden') + '</option>' +
          '</select> ' +
          '<i id="acl-spinner" class="icon-spinner hide"></i>' +
          '<i id="acl-error" class="icon-error hide"></i>' +
        '</div>' +
      '</form>'
    );

    $('#acl').val(Channels.value(id));

    Modal.current = 'acl';
    $('#modal').modal();
  },


  /*
   * Определение выбранного пункта в комбобоксе редактирования расширенных прав доступа.
   */
  value: function(id) {
    var acl = SimpleChat.match(Settings.getId(), id);

    if (acl & 9)
      return ACL_OWNER;

    var feed = SimpleChat.feed(Settings.getId(), FEED_NAME_ACL, 3);
    if (feed === false || !feed.hasOwnProperty(id))
      return -1;

    if (acl & 16)
      return ACL_MODERATOR;

    else if (acl == ACL_READWRITE || acl == ACL_READONLY || acl == ACL_FORBIDDEN)
      return acl;

    return -1;
  },


  /*
   * Установка нового заголовка канала.
   */
  setTitle: function(event) {
    event.preventDefault();

    var text = $('#title-edit').val();
    $('#modal').modal('hide');

    if ($('#channel-title-text').text() != text)
      SimpleChat.request(Settings.getId(), FEED_METHOD_POST, INFO_FEED_TITLE_REQ, {'value':text, 'options':7});
  },


  /*
   * Обработка изменения видимости канала.
   */
  setVisibility: function(event) {
    var value = $(this).find('option:selected').attr('value');
    SimpleChat.request(Settings.getId(), FEED_METHOD_POST, INFO_FEED_VISIBILITY_REQ, {'value':value, 'options':7});
    Channels.startSpinner('visibility');
  },


  /*
   * Установка прав доступа по умолчанию.
   */
  setDefaultAcl: function(event) {
    var value = $(this).find('option:selected').attr('value');
    SimpleChat.request(Settings.getId(), FEED_METHOD_PUT, ACL_FEED_HEAD_MASK_REQ, {'value':value,'options':6});

    if (value == ACL_CHANNEL_READWRITE)
      $('.sudo-control').fadeOut('fast')
    else
      $('.sudo-control').fadeIn('fast');
  },



  setSudoInvite: function(event) {
    SimpleChat.request(Settings.getId(), FEED_METHOD_POST, INFO_FEED_SUDO_REQ, {'value':$(this).is(':checked'), 'options':7});
  },


  /*
   * Обработка изменения закрепления канала.
   */
  pinChannel: function(event) {
    SimpleChat.request(Settings.getId(), FEED_METHOD_POST, INFO_FEED_PINNED_REQ, {'value':$(this).is(':checked'), 'options':7});
    Channels.startSpinner('pin');
  },


  /*
   * Обработка изменения журналирования канала.
   */
  logging: function(event) {
    SimpleChat.request(Settings.getId(), FEED_METHOD_POST, INFO_FEED_LOGGING_REQ, {'value':$(this).is(':checked'), 'options':7});
    Channels.startSpinner('logging');
  },


  setAcl: function(event) {
    var value = $(this).find('option:selected').attr('value');
    var user  = $(this).attr('data-user');
    var id    = Settings.getId();

    if (value == ACL_OWNER)
      SimpleChat.request(id, FEED_METHOD_POST,   ACL_FEED_HEAD_OWNER_REQ, {'value':user,'options':6});
    else if (value == ACL_DEFAULT)
      SimpleChat.request(id, FEED_METHOD_DELETE, ACL_FEED_HEAD_OTHER_REQ + '/' + user, {'options':6});
    else
      SimpleChat.request(id, FEED_METHOD_POST,   ACL_FEED_HEAD_OTHER_REQ + '/' + user, {'value':value,'options':6});

    Channels.startSpinner('acl');
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

  var feed = SimpleChat.feed(Settings.getId(), FEED_NAME_INFO, 4);
  if (feed !== false) {
    var visibility = feed.visibility || 0;
    var sudo       = feed.sudo       || false;
  }

  var body = $('#modal-body');
  body.append('<form>');
  body.append(
    '<div id="visibility-row" class="row">' +
      '<label for="visibility" data-tr="channels_visibility" class="options-label">' + Utils.tr('channels_visibility') + '</label> ' +
      '<select id="visibility" class="options-select">' +
        '<option value="0"  data-tr="channels_default">'                                                   + Utils.tr('channels_default')        + '</option>' +
        '<option value="1"  ' + (visibility > 0 ? 'selected' : '') + ' data-tr="channels_always_visible">' + Utils.tr('channels_always_visible') + '</option>' +
        '<option value="-1" ' + (visibility < 0 ? 'selected' : '') + ' data-tr="channels_hidden">'         + Utils.tr('channels_hidden')         + '</option>' +
      '</select> ' +
      '<i id="visibility-spinner" class="icon-spinner hide"></i>' +
      '<i id="visibility-error" class="icon-error hide"></i>' +
    '</div>'
  );

  var aclFeed = SimpleChat.feed(Settings.getId(), FEED_NAME_ACL, 3);
  if (aclFeed !== false)
    var acl = SimpleChat.aclToInt(aclFeed['*']);

  body.append(
    '<div id="permissions-row" class="row">' +
      '<label for="permissions" data-tr="channels_default_acl" class="options-label">' + Utils.tr('channels_default_acl') + '</label> ' +
      '<select id="permissions" class="options-select">' +
        '<option value="502" data-tr="channels_readwrite" ' + (acl == 6 ? 'selected' : '') + '>' + Utils.tr('channels_readwrite') + '</option>' +
        '<option value="484" data-tr="channels_readonly"  ' + (acl == 4 ? 'selected' : '') + '>' + Utils.tr('channels_readonly')  + '</option>' +
        '<option value="448" data-tr="channels_forbidden" ' + (acl == 0 ? 'selected' : '') + '>' + Utils.tr('channels_forbidden') + '</option>' +
      '</select> ' +
      '<input class="sudo-control" id="sudo" type="checkbox"' + (sudo === false ? '' : 'checked') + '> ' +
      '<label class="sudo-control" for="sudo" data-tr="channels_sudo_invite">' + Utils.tr('channels_sudo_invite') + '</label> ' +
    '</div>'
  );

  body.append(
    '<div id="logging-row" class="row">' +
      '<input id="logging" type="checkbox" ' + (feed[INFO_FEED_LOGGING_KEY] === false ? '' : 'checked') + '> ' +
      '<label for="logging" data-tr="channels_logging">' + Utils.tr('channels_logging') + '</label> ' +
      '<i id="logging-spinner" class="icon-spinner hide"></i>' +
      '<i id="logging-error" class="icon-error hide"></i>' +
    '</div>'
  );

  if (SimpleChat.match(SimpleChat.serverId(), SimpleChat.id()) & 9) {
    body.append(
      '<div id="pin-row" class="row">' +
        '<input id="pin" type="checkbox" ' + (feed.pinned == true ? 'checked' : '') + '> ' +
        '<label for="pin" data-tr="channels_pin">' + Utils.tr('channels_pin') + '</label> ' +
        '<i id="pin-spinner" class="icon-spinner hide"></i>' +
        '<i id="pin-error" class="icon-error hide"></i>' +
      '</div>'
    );
  }

  body.append('</form>');

  if (acl == 6)
    $('.sudo-control').hide();
};


Modal.hidden.options = function() {
  clearTimeout(Channels.timeout.visibility);
  clearTimeout(Channels.timeout.pin);
};


Modal.shown.options = function() {
  Utils.adjustWidth($('.options-label'));
  Utils.adjustWidth($('.options-select'));
}


$(document).ready(function() {
  $('#page-header').append('<div id="channel-title"><div id="channel-title-text"></div></div>');

  var modal = $('#modal-body');
  modal.on('click.title',        '#title-ok',    Channels.setTitle);
  modal.on('change.visibility',  '#visibility',  Channels.setVisibility);
  modal.on('change.permissions', '#permissions', Channels.setDefaultAcl);
  modal.on('change.pinned',      '#pin',         Channels.pinChannel);
  modal.on('change.logging',     '#logging',     Channels.logging);
  modal.on('change.sudo',        '#sudo',        Channels.setSudoInvite);

  modal.on('change.acl',         '#acl',         Channels.setAcl);

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
