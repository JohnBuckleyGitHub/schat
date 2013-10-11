(function() {
  'use strict';

  var create = window.schat.ui.createElement;

  function ImageUpload(oid) {
    this.cont = create('div', {id:oid, class:'cont image-upload panel panel-default'});

    var head  = this.cont.appendChild(create('div',    {class:'panel-heading'}));
    this.head      = head.appendChild(create('span',   {'data-tr':'share-upload-images'}));
    this.cancelBtn = head.appendChild(create('button', {type:'button', class:'btn btn-default btn-cancel btn-xs pull-right', 'data-tr':'cancel'}));

    var body = this.cont.appendChild(create('div', {class:'panel-body'}));
    this.progress = body.appendChild(create('div', {class:'progress'}))
                        .appendChild(create('div', {class:'progress-bar'}));
  }


  ImageUpload.prototype.setProgress = function(bytesSent, bytesTotal) {
    this.progress.style.width = Math.floor(bytesSent * 100 / bytesTotal) + '%';

    if (bytesSent == bytesTotal)
      this.tr(this.head, 'share-please-wait')
  };


  ImageUpload.prototype.setStatus = function(status) {
    var mod = (status === 200 ? 'success' : 'danger');

    this.cont.setAttribute('class', 'cont image-upload panel panel-' + mod);
    this.cancelBtn.setAttribute('class', 'btn btn-cancel btn-xs pull-right btn-' + mod);
    this.progress.setAttribute('class', 'progress-bar progress-bar-' + mod);

    if (status !== 200) {
      this.tr(this.cancelBtn, 'share-close');
      this.tr(this.head, 'share-upload-error');
    }
    else
      this.cancelBtn.style.display = 'none';
  };


  ImageUpload.prototype.tr = function(el, key) {
    el.setAttribute('data-tr', key);
    el.innerHTML = tr(key);
  };

  window.schat.ui.ImageUpload = ImageUpload;
})();