(function() {
  'use strict';

  var create = window.schat.ui.createElement;

  function ImageMessage(json) {
    this.cont = create('div', {id:json.OID, class:'cont image-type panel panel-default', 'data-mdate':json.MDate, 'data-time':json.Date, 'data-remover':'image'});
    var extra = json.Status === 600 ? ' removed' : '';

    var head = this.cont.appendChild(create('div', {class:'panel-heading' + extra}));
    head.innerHTML = DateTime.template(json.Date, json.Day) + Messages.nameBlock(json.Author);

    var body = this.cont.appendChild(create('div', {class:'panel-body' + extra}));
    if (json.Status === 600) {
      body.appendChild(create('img', {class:'img-thumbnail', src:'qrc:/images/Share/removed-image.png', draggable:false}));
      return;
    }

    var data = json.JSON;
    if (data.format !== 1 || !schat.utils.isArray(data.images))
      return;

    var images = data.images;
    var a;

    for (var i = 0; i < images.length; i++) {
      a = body.appendChild(create('a', {
        href:this.imageUrl(data.link, images[i], 'O'),
        class:'img-thumbnail modal-toggle',
        style:"background: url('" + this.imageUrl(data.link, images[i], 'S') + "') no-repeat center;",
        'data-handler':'image'
      }));

      a.addEventListener('dragstart', this.onDragStart);
    }

    if (json.Text.length)
      head.appendChild(create('span', {class:'img-desc'}, json.Text));
  }


  ImageMessage.prototype.imageUrl = function(link, image, size) {
    return link + '/' + size + '/' + image.id.substr(0, 2) + '/' + image.id + '.' + image.type;
  };


  ImageMessage.prototype.onDragStart = function(event) {
    event.dataTransfer.setData('text/plain', event.target.href);
  };

  schat.ui.ImageMessage = ImageMessage;
})();