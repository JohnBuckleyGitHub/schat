(function() {
  'use strict';

  var title  = schat.ui.modal.title;
  var footer = schat.ui.modal.footer;
  var body   = schat.ui.modal.body;
  var create = window.schat.ui.createElement;

  function ShareImage(data) {
    ChatView.allowFocus(true);
    title.innerHTML = data.title;
    title.title     = data.title;

    this.id = data.id;

    var form = body.appendChild(create('form'));
    var group = form.appendChild(create('div', {class:'form-group'}));
    group.appendChild(create('label', {for:'img-desc', 'data-tr':'share-img-desc'}));
    var desc = group.appendChild(create('input', {type:'text', class:'form-control', id:'img-desc', autofocus:'autofocus'}));

    var upload = footer.appendChild(create('button', {type:'button', class:'btn btn-primary', 'data-tr':'share-upload'}));
    footer.appendChild(create('button', {type:'button', class:'btn btn-default', 'data-dismiss':'modal', 'data-tr':'cancel'}));

    form.addEventListener('submit', function(event) {
      event.preventDefault();
      upload.click();
    });

    var that = this;

    upload.addEventListener('click', function(event) {
      Share.upload(that.id, desc.value);
      that.id = null;
      
      $(schat.ui.modal.element).modal('hide');
    });
  }


  ShareImage.prototype.hidden = function() {
    ChatView.allowFocus(false);
    title.title = '';

    if (this.id)
      Share.cancel(this.id);
  };


  Share.add = function(data) {
    $(schat.ui.modal.element).modal('hide');

    schat.ui.modal.current = new ShareImage(data);
    $(schat.ui.modal.element).modal();
  }
})();