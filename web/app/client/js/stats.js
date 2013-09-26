(function(){
  'use strict';

  var client = schat.net.client;
  var widget = null;

  function stats(reply) {
    if (schat.page !== 'stats')
      return;

    if (widget === null)
      widget = new schat.ui.StatsWidget();

    widget.setStats(reply.data[0]);
  }

  function get() {
    if (client.socket.readyState != 1)
      return false;

    client.on('res.' + client.serverId + '/stats', stats);
    client.get(client.serverId + '/stats', stats);
    return true;
  }


  function open() {
    if (schat.page !== 'stats')
      return;

    get();
    schat.ui.Loader.hide();
  }


  function route(period) {
    schat.page = 'stats';

    if (!period || (period !== 'hour' && period !== 'day' && period !== 'week' && period !== 'month' && period !== 'year')) {
      period = localStorage.getItem('statsPeriod');
      period = period || 'week';
      schat.router.navigate('/stats/' + period, false, true);
    }

    localStorage.setItem('statsPeriod', period);
    get();

    if (widget === null)
      widget = new schat.ui.StatsWidget();

    widget.setPeriod(period);
  }


  function unload() {
    if (widget)
      widget.clear();

    client.off('res.' + client.serverId + '/stats', stats);
    widget = null;
  }

  schat.router.route('/stats', route, unload);
  schat.router.route('/stats/:period', route, unload);
  client.on('open', open);
})();