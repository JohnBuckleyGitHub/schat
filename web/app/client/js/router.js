(function() {
  'use strict';

  var namedParam = /:\w+/g;
  var splatParam = /\*\w+/g;

  function Router() {
    this.routes  = {};
    this.leave   = null;
    this.current = '';

    // BUG: https://code.google.com/p/chromium/issues/detail?id=63040
    window.addEventListener('load', function() {
      setTimeout(function() {
        window.addEventListener('popstate', function() {
          this.checkRoutes(document.location.pathname);
        }.bind(this));
      }.bind(this), 0);
    }.bind(this));
  }

  Router.prototype.route = function(route, callback, leave) {
    route = route.replace(namedParam, '([^\/]+)').replace(splatParam, '(.*?)');
    leave = leave || null;
    this.routes["^" + route + "$"] = [callback, leave];
  };

  Router.prototype.navigate = function(url, trigger, replace) {
    if (replace === true)
      window.history.replaceState({url:url}, '', url);
    else
      window.history.pushState({url:url}, '', url);

    if (trigger !== false)
      this.checkRoutes(url);
  };

  Router.prototype.checkRoutes = function(url) {
    var regex;
    var key  = '^$';
    var args = [];

    for (var regexText in this.routes) if (this.routes.hasOwnProperty(regexText)) {
      regex = new RegExp(regexText);
      if (regex.test(url)) {
        key  = regexText;
        args = regex.exec(url).slice(1);
        break;
      }
    }

    if (this.leave !== null && this.current !== key)
      this.leave();

    this.routes[key][0].apply(window, args);
    this.leave   = this.routes[key][1];
    this.current = key;
  };

  schat.router = new Router();
})();