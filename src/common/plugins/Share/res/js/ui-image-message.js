(function() {
  'use strict';

  var create = window.schat.ui.createElement;

  function ImageMessage(json) {
    this.cont = create('div', {id:json.OID, class:'cont image-type panel panel-default'});

    var head = this.cont.appendChild(create('div', {class:'panel-heading'}));
    head.innerHTML = DateTime.template(json.Date, json.Day) + Messages.nameBlock(json.Author);

    var body = this.cont.appendChild(create('div', {class:'panel-body'}));

    var data = json.JSON;
    if (data.format !== 1 || !schat.utils.isArray(data.images))
      return;

    var images = data.images;
    for (var i = 0; i < images.length; i++) {
      body.appendChild(create('img', {class:'img-thumbnail', src:data.link + '/S/' + images[i].id.substr(0, 2) + '/' + images[i].id + '.' + images[i].type}));
    }
  }

  schat.ui.ImageMessage = ImageMessage;
})();