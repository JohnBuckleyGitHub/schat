(function(){
  'use strict';

  var PAGE   = 'user';
  var client = schat.net.client;
  var widget = null;
  var userId = '';

  function user(reply) {
    if (schat.page !== PAGE)
      return;

    if (reply.status === 200 && reply.data[0].type === 117) {
      if (widget === null)
        widget = new schat.ui.UserWidget();

      widget.setUser(reply.data[0]);
    }
    else
      error('user-not-found');
  }

  function get() {
    if (client.socket.readyState != 1)
      return false;

    client.on('res.' + userId, user);
    client.get(userId, user);
    return true;
  }


  function open() {
    if (schat.page !== PAGE)
      return;

    get();
    schat.ui.Loader.hide();
  }


  function route(id) {
    schat.page = PAGE;

    if (!id || id.length != 34) {
      error('user-not-found');
      return;
    }

    userId = id;
    get();
  }


  function unload() {
    widget = null;
  }


  function error(text) {
    schat.ui.main.textContent = '';
    schat.ui.main.setAttribute('class', 'panel-main');
    schat.ui.main.appendChild(schat.ui.createElement('div', {class:'alert alert-danger', 'data-tr':text}));
  }

  schat.router.route('/user/:id', route, unload);
  client.on('open', open);
})();