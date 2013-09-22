(function() {
  'use strict';

  var create = schat.ui.createElement;

  function UsersWidget() {
    schat.ui.main.textContent = '';
    schat.ui.main.setAttribute('class', 'panel-main');

    var panel = create('div', {class:'panel panel-default panel-users'});

    this.list = new schat.ui.UserList(panel.appendChild(create('ul', {class:'list-group'})));

    schat.ui.main.appendChild(panel);
  }


  UsersWidget.prototype.add = function(user) {
    this.list.add(user);
  };

  schat.ui.UsersWidget = UsersWidget;
})();