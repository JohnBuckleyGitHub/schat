(function() {
  'use strict';

  var create = window.schat.ui.createElement;

  function ImageMessage(oid) {
    this.cont = create('div', {id:oid, class:'cont image-type panel panel-default'});

    var head = this.cont.appendChild(create('div', {class:'panel-heading'}));
    head.appendChild(create('span', {'data-tr':'share-upload-images'}));
    this.cancelBtn = head.appendChild(create('button', {type:'button', class:'btn btn-default btn-cancel btn-xs pull-right', 'data-tr':'cancel'}));

    var body = this.cont.appendChild(create('div', {class:'panel-body'}));
    this.progress = body.appendChild(create('div', {class:'progress'}))
                        .appendChild(create('div', {class:'progress-bar'}));
  }


  ImageMessage.prototype.setProgress = function(bytesSent, bytesTotal) {
    this.progress.style.width = Math.floor(bytesSent * 100 / bytesTotal) + '%';
  };

  window.schat.ui.ImageMessage = ImageMessage;
})();