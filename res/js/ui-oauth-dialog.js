(function() {
  'use strict';

  var title  = schat.ui.modal.title;
  var footer = schat.ui.modal.footer;
  var body   = schat.ui.modal.body;
  var create = window.schat.ui.createElement;

  function OAuthDialog() {
    title.innerHTML = '<span data-tr="sign_in_with">' + tr('sign_in_with') + '</span> <i class="icon-spinner"></i>';
    footer.style.display = 'none';

    this.providers = body.appendChild(create('div', {id:'providers'}));

    if (Auth.anonymous()) {
      var anonymous = body.appendChild(create('div')).appendChild(create('a', {'data-tr':'or_anon_connect', href:'#'}));

      $(anonymous).on('click', function(event) {
        event.preventDefault();
        Auth.open();
      });
    }

    AuthDialog.providers();
  }


  OAuthDialog.prototype.shown = function() {
    Utils.adjustWidth($('#providers .btn'));
  };


  OAuthDialog.prototype.hidden = function() {
    Auth.cancel();
  };

  window.schat.ui.OAuthDialog = OAuthDialog;
})();