(function() {
  'use strict';

  var OAuthWidget = function OAuthWidget(providers, state) {
    this.widget = schat.ui.createElement('div', {class:'center-widget oauth-widget'});
    this.widget.appendChild(schat.ui.createElement('h3', {'data-tr':'oauth-header'}));

    var links = [];
    var link;

    for (var i = 0; i < providers.length; i++) {
      link = schat.ui.createElement(
          'a',
          {class:'btn btn-default', href:providers[i][3].replace('${STATE}', state)},
          '<i class="provider provider-' + providers[i][0] + '"></i> ' + providers[i][2]
      );

      links.push(link);
      this.widget.appendChild(link);
    }

    schat.ui.main.textContent = '';
    schat.ui.main.setAttribute('class', 'center-wrapper');
    schat.ui.main.appendChild(this.widget);

    this.adjustWidth(links);
  };

  OAuthWidget.prototype.adjustWidth = function(links) {
    var max = 0;
    var current = 0;

    for (var i = 0; i < links.length; i++) {
      current = links[i].offsetWidth;
      if (current > max)
        max = current;
    }

    for (i = 0; i < links.length; i++)
      links[i].style.width = max + 'px';

    this.widget.style.maxWidth = (max * 3 + 50) + 'px';
  };

  window.schat.ui.OAuthWidget = OAuthWidget;
})();