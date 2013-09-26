(function(){
  'use strict';

  var client = schat.net.client;
  var widget = null;

  function open() {
    if (schat.page != 'index')
      return;

    get();
  }


  /**
   * Запрос данных.
   *
   * @returns {boolean}
   */
  function get() {
    if (client.socket.readyState != 1)
      return false;

    client.on('res.' + client.serverId + '/index', rooms);
    client.get(client.serverId + '/index', rooms);

    client.on('res.' + client.serverId + '/stats', stats);
    client.get(client.serverId + '/stats', stats);
    return true;
  }


  /**
   * Обработка получения списка комнат.
   *
   * @param reply
   */
  function rooms(reply) {
    if (schat.page != 'index')
      return;

    if (reply.status === 200) {
      if (widget === null)
        widget = new schat.ui.MainWidget();

      widget.date.index = reply.date;
      widget.setRooms(reply.data);
    }

    hideLoader();
  }


  /**
   * Обработка получения статистики.
   *
   * @param reply
   */
  function stats(reply) {
    if (schat.page !== 'index')
      return;

    if (reply.status === 200) {
      if (widget === null)
        widget = new schat.ui.MainWidget();

      widget.date.stats = reply.date;
      widget.setStats(reply.data[0]);
    }

    hideLoader();
  }


  function hideLoader() {
    if (widget && widget.date.index && widget.date.stats)
      schat.ui.Loader.hide();
  }


  function route() {
    schat.page = 'index';
    schat.router.navigate('/', false, true);

    schat.ui.Loader.show();
    get();
  }


  function unload() {
    client.off('res.' + client.serverId + '/index', rooms);
    client.off('res.' + client.serverId + '/stats', stats);

    widget = null;
  }


  schat.router.route('', route, unload);
  client.on('open', open);
})();