(function() {
  'use strict';

  var title  = schat.ui.modal.title;
  var footer = schat.ui.modal.footer;
  var body   = schat.ui.modal.body;
  var create = window.schat.ui.createElement;

  function ImageDialog(image) {
    title.innerHTML = '&nbsp;';
    body.appendChild(create('img', {src:image, class:'img-responsive', style:'margin: 0 auto'}));
    footer.style.display = 'none';

    $(schat.ui.modal.dialog).addClass('image-dialog');
  }


  ImageDialog.prototype.hidden = function() {
    $(schat.ui.modal.dialog).removeClass('image-dialog');
  };


  schat.ui.ImageDialog = ImageDialog;
})();