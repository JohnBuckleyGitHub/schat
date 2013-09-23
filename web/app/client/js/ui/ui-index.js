(function() {
  'use strict';

  function MainWidget() {
    schat.ui.main.textContent = '';
    schat.ui.main.setAttribute('class', 'panel-main');
    this.qt = typeof SimpleChat !== 'undefined';

    this.date = {
      index: 0,
      stats: 0
    };

    var panel = schat.ui.createElement('div', {class:'panel panel-default panel-rooms'});

    if (!this.qt)
      panel.appendChild(schat.ui.createElement('div', {class:'panel-heading','data-tr':'common-rooms'}));

    this.rooms = schat.ui.createElement('div', {class:'list-group'});
    panel.appendChild(this.rooms);
    schat.ui.main.appendChild(panel);

    if (!this.qt) {
      panel = schat.ui.createElement('div', {class:'panel panel-default panel-stats'});
      panel.appendChild(schat.ui.createElement('div', {class:'panel-heading'},
          (this.qt ? tr('common-stats') : '<a href="/stats" class="internal" data-tr="common-stats">' + tr('common-stats') + '</a>')
      ));

      this.stats = panel.appendChild(schat.ui.createElement('div', {class:'panel-body'}));
      schat.ui.main.appendChild(panel);
    }
  }


  MainWidget.prototype.setRooms = function(rooms) {
    this.rooms.textContent = '';

    for (var i = 0; i < rooms.length; i++) {
      var options = rooms[i][4];

      this.rooms.appendChild(schat.ui.createElement('div', {class:'list-group-item'},
          '<span class="badge">' + rooms[i][2] + '</span>' +
              ((options & 2) ? '<i class="schat-icon schat-icon-pin pull-right"></i>' : '') +
              ((options & 4) ? '<i class="schat-icon schat-icon-lock pull-right"></i>' : '') +
              '<h5 class="list-group-item-heading">' + this.toUrl(rooms[i][0], rooms[i][1], options) + '</h5>' +
              '<small class="list-group-item-text text-muted">' + (rooms[i][3] === '' ? tr('channels-no-title') : schat.utils.encode(rooms[i][3]) ) + '</small>'
      ));
    }
  };


  MainWidget.prototype.setStats = function(stats) {
    stats = stats[0];

    this.stats.innerHTML = schat.utils.format(tr('stats-stats'),
        [
          tr('stats-users', stats.users[0]),
          tr('stats-rooms', stats.rooms[0]),
          stats.users[1],
          stats.users[2]
        ]);

    $('.timeago').timeago();
  };


  MainWidget.prototype.toUrl = function(id, name, options) {
    return '<a class="internal" href="/talk/' + id + '">' + schat.utils.encode(name) + '</a>';
  };

  window.schat.ui.MainWidget = MainWidget;
})();