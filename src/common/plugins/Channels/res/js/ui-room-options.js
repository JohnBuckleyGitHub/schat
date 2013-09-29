(function() {
  'use strict';

  var title  = schat.ui.modal.title;
  var footer = schat.ui.modal.footer;
  var body   = schat.ui.modal.body;
  var create = window.schat.ui.createElement;

  /**
   * Возвращает true если пользователь может закрепить канал.
   *
   * \param visibility Текущие настройки видимости канала.
   */
  function isPinnable(visibility) {
    return (visibility >= 0 && SimpleChat.match(SimpleChat.serverId(), SimpleChat.id()) & 9) > 0;
  }


  function RoomOptionsDialog() {
    title.textContent = tr('channels-options');
    title.setAttribute('data-tr', 'channels-options');
    footer.style.display = 'none';

    var that = this;
    SimpleChat.feed(Settings.getId(), FEED_NAME_INFO, 1);

    $.ajax({
      url: 'channel_options.html',
      isLocal: true,
      dataType: 'html',
      success: function() { that.onLoadSuccess.apply(that, arguments); }
    });
  }


  RoomOptionsDialog.prototype.onLoadSuccess = function(html) {
    body.innerHTML = html;

    $('#channel-options [data-tr]').each(function() {
      TR($(this).attr('data-tr'));
    });

    this.visibility     = document.getElementById('visibility');
    this.visibilityRow  = document.getElementById('visibility-row');
    this.permissions    = document.getElementById('permissions');
    this.permissionsRow = document.getElementById('permissions-row');
    this.logging        = document.getElementById('logging');
    this.pinBtn         = document.getElementById('pin-button');
    this.sudoBtn        = document.getElementById('sudo-button');

    this.retranslate();
    this.reload();

    var that = this;

    $(this.visibility).on('change',  function() { that.setVisibility.apply(that, arguments); });
    $(this.permissions).on('change', function() { that.setPermissions.apply(that, arguments); });
    $(this.logging).on('change',     this.setLogging);
    $(this.pinBtn).on('click',       this.pin);
    $(this.sudoBtn).on('click',      this.sudo);
  };


  /**
   * Динамический перевод интерфейса.
   */
  RoomOptionsDialog.prototype.retranslate = function() {
    Utils.adjustWidth($('.options-label'));
    Utils.adjustWidth($('.options-select'));

    $(this.pinBtn).attr('title', Utils.tr('channels-pin'));
    $(this.sudoBtn).attr('title', Utils.tr('channels-sudo-invite'));
  };


  /**
   * Обновление состояния диалога в зависимости от текущих настроек.
   */
  RoomOptionsDialog.prototype.reload = function() {
    var feed = SimpleChat.feed(Settings.getId(), FEED_NAME_INFO, 3);
    if (feed !== false) {
      var visibility = feed.visibility || 0;
      var sudo       = feed.sudo       || false;
    }

    this.visibility.value = visibility;
    $(this.visibilityRow).toggleClass('pinnable', isPinnable(visibility));

    var acl = SimpleChat.match(Settings.getId(), '');
    var permissions = ACL_CHANNEL_READWRITE;
    if (acl == 4)
      permissions = ACL_CHANNEL_READONLY;
    else if (acl == 0)
      permissions = ACL_CHANNEL_FORBIDDEN;

    this.permissions.value = permissions;
    $(this.permissionsRow).toggleClass('strict-access', permissions != ACL_CHANNEL_READWRITE);
    $(this.pinBtn).toggleClass('active', feed[INFO_FEED_PINNED_KEY] == true);
    $(this.sudoBtn).toggleClass('active', sudo !== false);
    $(this.logging).attr('checked', feed[INFO_FEED_LOGGING_KEY] !== false)
  };


  /**
   * Установка видимости канала.
   */
  RoomOptionsDialog.prototype.setVisibility = function() {
    var value = this.visibility.value;
    $(this.visibilityRow).toggleClass('pinnable', isPinnable(value));

    SimpleChat.request(Settings.getId(), FEED_METHOD_POST, INFO_FEED_VISIBILITY_REQ, {'value':value, 'options':7});
  };


  /**
   * Установка прав доступа по умолчанию.
   */
  RoomOptionsDialog.prototype.setPermissions = function() {
    var value = this.permissions.value;
    SimpleChat.request(Settings.getId(), FEED_METHOD_PUT, ACL_FEED_HEAD_MASK_REQ, {'value':value,'options':6});

    $(this.permissionsRow).toggleClass('strict-access', value != ACL_CHANNEL_READWRITE);
  };


  /**
   * Обработка изменения журналирования канала.
   */
  RoomOptionsDialog.prototype.setLogging = function() {
    SimpleChat.request(Settings.getId(), FEED_METHOD_POST, INFO_FEED_LOGGING_REQ, {'value':$(this).is(':checked'), 'options':7});
  };


  /**
   * Обработка изменения закрепления канала.
   */
  RoomOptionsDialog.prototype.pin = function() {
    var btn = $(this);
    btn.toggleClass('active');
    SimpleChat.request(Settings.getId(), FEED_METHOD_POST, INFO_FEED_PINNED_REQ, {'value':btn.hasClass('active'), 'options':7});
  };


  /**
   * Обработка изменения режима приглашения в канал.
   */
  RoomOptionsDialog.prototype.sudo = function() {
    var btn = $(this);
    btn.toggleClass('active');
    SimpleChat.request(Settings.getId(), FEED_METHOD_POST, INFO_FEED_SUDO_REQ, {'value':btn.hasClass('active'), 'options':7});
  };

  schat.ui.RoomOptionsDialog = RoomOptionsDialog;

  if (typeof SimpleChat !== 'undefined') {
    SimpleChat.retranslated.connect(function() {
      var dialog = schat.ui.modal.current;
      if (dialog instanceof schat.ui.RoomOptionsDialog) {
        dialog.retranslate.apply(dialog);
      }
    });
  }
})();