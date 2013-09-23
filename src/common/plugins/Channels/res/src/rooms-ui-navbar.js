(function() {
  'use strict';

  var create = schat.ui.createElement;

  function RoomsNavbar() {
    console.log('RoomsNavbar');

    this.serverName = document.getElementById('server-name');

    console.log(this.serverName);

//    schat.ui.main.textContent = '';
//    schat.ui.main.setAttribute('class', 'panel-main');
//
//    var panel = create('div', {class:'panel panel-default panel-stats'});
//
//    var title = panel.appendChild(create('div', {class:'panel-heading'})).appendChild(create('h3'));
//    title.appendChild(create('span', {'data-tr':'stats-title'}));
//    title.appendChild(document.createTextNode(' '));
//
//    var body  = panel.appendChild(create('div', {class:'panel-body'}));
//
//    this.interval = null;
//    this.subtitle = title.appendChild(create('small'));
//    this.graph    = body.appendChild(create('img', {src:'/img/stats/users-day.png', class:'img-responsive img-stats-graph'}));
//    this.pager    = body.appendChild(create('ul', {class:'pager'}));
//
//    this.pager.innerHTML =
//        '<li><a class="internal" href="/stats/hour" data-tr="common-hour">' + tr('common-hour') + '</a></li> ' +
//            '<li><a class="internal" href="/stats/day" data-tr="common-day">' + tr('common-day') + '</a></li> ' +
//            '<li><a class="internal" href="/stats/week" data-tr="common-week">' + tr('common-week') + '</a></li> ' +
//            '<li><a class="internal" href="/stats/month" data-tr="common-month">' + tr('common-month') + '</a></li>';
//
//    schat.ui.main.appendChild(panel);
//
//    panel = create('div', {class:'panel panel-default panel-stats'});
//    this.stats = panel.appendChild(create('div', {class:'panel-body'}));
//    schat.ui.main.appendChild(panel);
  }

  RoomsNavbar.prototype.setName = function(name) {
    console.log(this, name);

    this.serverName.textContent = name;
  };

  window.schat.ui.RoomsNavbar = RoomsNavbar;
})();