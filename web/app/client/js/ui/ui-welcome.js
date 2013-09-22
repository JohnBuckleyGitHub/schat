(function() {
  'use strict';

  function WelcomeWidget(info) {
    var user   = info.user  || {};
    user.name  = user.name  || '';
    user.email = user.email || '';

    this.widget = schat.ui.createElement('form', {class:'center-widget signin-widget clearfix'});
    this.widget.appendChild(schat.ui.createElement('button', {type:'button', class:'close', 'data-dismiss':'alert','aria-hidden':true}, '&times;'));

    this.widget.appendChild(schat.ui.createElement('div', {class:'form-group'},
        '<label for="nick-input" data-tr="welcome-nick">' + tr('common-nick') + '</label>' +
        '<div class="input-group">' +
          '<input type="text" class="form-control" id="nick-input" placeholder="' + tr('common-your-nick') + '" value="' + user.name + '">' +
          '<div class="input-group-btn">' +
            '<button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown"><span data-tr="common-male">' + tr('common-male') + '</span> <span class="caret"></span></button>' +
            '<ul class="dropdown-menu pull-right">' +
              '<li><a href="#" data-tr="common-male">' + tr('common-male') + '</a></li>' +
              '<li><a href="#" data-tr="common-female">' + tr('common-female') + '</a></li>' +
            '</ul>' +
          '</div>' +
        '</div>'
    ));

    this.widget.appendChild(schat.ui.createElement('div', {class:'form-group'},
        '<label for="email-input" data-tr="common-email">' + tr('common-email') + '</label>' +
        '<input type="text" class="form-control" id="email-input" placeholder="' + tr('common-your-email') + '" value="' + user.email + '">'
    ));

    this.widget.appendChild(schat.ui.createElement('button', {type:'submit', class:'btn btn-primary pull-right', 'data-tr':'common-signin'}));

    schat.ui.main.textContent = '';
    schat.ui.main.setAttribute('class', 'center-wrapper');
    schat.ui.main.appendChild(this.widget);
  }

  window.schat.ui.WelcomeWidget = WelcomeWidget;
})();