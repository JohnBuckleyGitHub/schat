(function() {
  'use strict';

  var title  = schat.ui.modal.title;
  var footer = schat.ui.modal.footer;
  var body   = schat.ui.modal.body;
  var create = window.schat.ui.createElement;

  function ConnectionDialog(host, id) {
    title.textContent = host;
    footer.style.display = 'none';

    var feed = SimpleChat.feed(Settings.id, FEED_NAME_USER, 3);
    if (feed === false || !feed.hasOwnProperty('connections'))
      return;

    var json = typeof id === 'undefined' ? feed.last : feed.connections[id];
    if (typeof json === 'undefined')
      return;

    title.textContent = json.host;
    body.innerHTML += Utils.row('chat_version', schat.utils.encode(json.version));
    body.innerHTML += Utils.row('os_name', '<i class="icon-os os-' + Pages.os(json.os) + '"></i> ' + schat.utils.encode(json.osName));

    for (var i = 0; i < UserHooks.connection.length; i++)
      UserHooks.connection[i](json);
  }


  ConnectionDialog.prototype.shown = function() {
    Utils.adjustWidth($('#modal .field-row-label'));
  };

  window.schat.ui.ConnectionDialog = ConnectionDialog;
})();