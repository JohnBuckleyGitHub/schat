(function(){
  'use strict';

  function UserList(element) {
    this.el    = element;
    this.users = {};
    this.list  = [];
  }


  UserList.prototype.add = function(user) {
    this.users[user.id] = user;
    this.list.push(user.id);

    user.el = this.el.appendChild(schat.ui.createElement('li', {class:'list-group-item'},
        '<a class="internal" href="/user/' + user.id + '">' + user.name + '</a>'
    ));
  };


  UserList.prototype.sort = function() {
    this.list.sort(function(a, b) {
      a = this.users[a].name.toLowerCase();
      b = this.users[b].name.toLowerCase();

      if (a < b)
        return -1;

      if (a > b)
        return 1;

      return 0;
    }.bind(this));

    var user;
    for (var i = 0; i < this.list.length; i++) {
      user    = this.users[this.list[i]];
      user.el = this.el.appendChild(user.el);
    }
  };

  schat.ui.UserList = UserList;
})();