(function() {
  'use strict';

  var title  = schat.ui.modal.title;
  var footer = schat.ui.modal.footer;
  var body   = schat.ui.modal.body;
  var create = window.schat.ui.createElement;

  function ShareImage() {
    title.innerHTML = '&nbsp;';
    var dragArea = body.appendChild(create('div', {'style':'border: 1px solid #ff0000; height: 69px;'}));
    dragArea.addEventListener('dragover', function(event) {
      console.log('dragover', event);
    });

    dragArea.addEventListener('drop', function(event) {
      console.log('drop', event);
    });

    dragArea.ondrop = function() {
      console.log('O_i');
    }
  }


  Share.add = function() {
    $(schat.ui.modal.element).modal('hide');

    schat.ui.modal.current = new ShareImage();
    $(schat.ui.modal.element).modal();
  }
})();