(function() {
  'use strict';

  var create = schat.ui.createElement;

  function UserWidget() {
    schat.ui.main.textContent = '';
    schat.ui.main.setAttribute('class', 'panel-main');

    var panel = create('div', {class:'panel panel-default panel-user'});

    var title = panel.appendChild(create('div', {class:'panel-heading'})).appendChild(create('h3'));
    this.nick = title.appendChild(create('span'));
    title.appendChild(document.createTextNode(' '));
    this.status = title.appendChild(create('small'));

    panel.appendChild(create('div', {class:'panel-body'}));

    schat.ui.main.appendChild(panel);
  }


  UserWidget.prototype.setUser = function(data) {
    this.nick.textContent = data.name;

    var status = schat.ui.user.status(data.status);
    this.status.setAttribute('data-tr', status);
    this.status.textContent = tr(status);
  };


  schat.ui.UserWidget = UserWidget;

  schat.ui.user = schat.ui.user || {};
  schat.ui.user.status = function(status) {
    status = status || 0;

    switch (status) {
      case 0:
        return 'user-status-offline';

      case 1:
        return 'user-status-online';

      case 2:
      case 3:
        return 'user-status-away';

      case 4:
        return 'user-status-dnd';

      case 5:
        return 'user-status-free';

      default:
        return '';
    }
  };
})();