(function(){
  'use strict';

  var PAGE   = 'users';
  var client = schat.net.client;
  var widget = null;

  function user(reply, req) {
    if (schat.page !== PAGE)
      return;

    console.log('USERE', reply, req);

    var data = reply.data[0];
    data.id  = req;

    widget.add(data);
  }


  function users(reply) {
    if (schat.page !== PAGE)
      return;

    if (widget === null)
      widget = new schat.ui.UsersWidget();

    if (reply.status === 200 && schat.utils.isArray(reply.data)) {
      var list = reply.data;
      for (var i = 0; i < list.length; i++)
        client.get(list[i], user);
    }

    schat.ui.Loader.hide();
  }


  function get() {
    if (client.socket.readyState != 1)
      return false;

    client.get(client.serverId + '/users', users);
    return true;
  }


  function open() {
    if (schat.page !== PAGE)
      return;

    get();
  }


  function route() {
    schat.page = PAGE;

    get();
  }


  function unload() {
    widget = null;
  }


  schat.router.route('/users', route, unload);
  client.on('open', open);
})();