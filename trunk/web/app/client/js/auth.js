(function(){
  'use strict';

  var Auth = {
    widget: null
  };

  /**
   * Обработка получения списка провайдеров.
   * @param reply
   */
  Auth.providers = function(reply) {
    if (schat.page !== 'signin')
      return;

    if (reply.status !== 200)
      return;

    this.widget = new schat.ui.OAuthWidget(reply.data[0], reply.data[1]);
    localStorage.setItem('auth', JSON.stringify({state:reply.data[1].substring(0, 34), secret:reply.data[2]}));
    schat.ui.Loader.hide();
  };

  /**
   * Обработка получения состояния авторизации.
   * @param reply
   */
  Auth.state = function(reply) {
    if (schat.page !== 'welcome')
      return;

    if (reply.status !== 200)
      return;

    this.widget = new schat.ui.WelcomeWidget(reply.data[0]);
    schat.ui.Loader.hide();
  };

  Auth.open = function() {
    if (schat.page == 'signin')
      this.getProviders();
    else if (schat.page == 'welcome')
      this.getState();
  };


  /**
   * Запрос списка провайдеров.
   *
   * @returns {boolean}
   */
  Auth.getProviders = function() {
    if (schat.net.client.socket.readyState != 1)
      return false;

    schat.net.client.send(
        {
          method:'AUTH',
          request:'providers',
          data:window.location.protocol + '//' + window.location.host + '/w'
        }, Auth.providers.bind(Auth));

    return true;
  };


  /**
   * Запрос состояния авторизации.
   *
   * @returns {boolean}
   */
  Auth.getState = function() {
    if (schat.net.client.socket.readyState != 1)
      return false;

    var auth = null;
    try {
      auth = JSON.parse(localStorage.getItem('auth'));
    }
    catch (e) {}

    if (auth === null) {
      schat.router.navigate('/signin');
      return false;
    }

    schat.net.client.send({method:'AUTH', request:'state', data:[auth.state, auth.secret]}, Auth.state.bind(this));
    return true;
  };


  schat.router.route('/signin', function() {
    schat.page = 'signin';

    Auth.getProviders();
    schat.ui.Loader.show();
  });


  schat.router.route('/w', function() {
    schat.page = 'welcome';
    schat.router.navigate('/welcome', true, true);
  });


  schat.router.route('/welcome', function() {
    schat.page = 'welcome';

    Auth.getState();
    schat.ui.Loader.show();
  });

  schat.net.client.on('open', Auth.open.bind(Auth));
})();