(function() {
  'use strict';

  function RoomsNavbar() {
    this.serverName = document.getElementById('server-name');
    this.joinName   = document.getElementById('join-name');
  }

  RoomsNavbar.prototype.setName = function(name) {
    this.serverName.textContent = name;
  };

  window.schat.ui.RoomsNavbar = RoomsNavbar;
})();