(function() {
  'use strict';

  var title  = schat.ui.modal.title;
  var footer = schat.ui.modal.footer;
  var body   = schat.ui.modal.body;
  var create = window.schat.ui.createElement;

  function RoomTitleDialog() {
    title.textContent = tr('channels-title');
    title.setAttribute('data-tr', 'channels-title');
    ChatView.allowFocus(true);

    body.innerHTML =
        '<div class="form-horizontal">' +
          '<div class="form-group">' +
            '<div class="col-sm-12">' +
              '<input type="text" class="form-control" id="title-edit" maxlength="200" autofocus="autofocus">' +
            '</div>' +
          '</div>' +
        '</div>';

    this.btn = footer.appendChild(create('button', {type:'button', class:'btn btn-primary', 'data-tr':'ok'}));
    footer.appendChild(create('button', {type:'button', class:'btn btn-default', 'data-dismiss':'modal', 'data-tr':'cancel'}));

    this.titleEdit = document.getElementById('title-edit');
    this.titleEdit.value = document.getElementById('channel-title-text').textContent;

    var that = this;

    $(this.titleEdit).keydown(function(event){
      if(event.keyCode == 13)
        that.btn.click();
    });

    $(this.btn).on('click', function() { that.setTitle.apply(that, arguments); });
  }


  RoomTitleDialog.prototype.hidden = function() {
    ChatView.allowFocus(false);
  };


  RoomTitleDialog.prototype.setTitle = function(event) {
    event.preventDefault();

    var text = this.titleEdit.value;
    if (document.getElementById('channel-title-text').textContent !== text)
      SimpleChat.request(Settings.getId(), FEED_METHOD_POST, INFO_FEED_TITLE_REQ, {'value':text, 'options':7});

    $(schat.ui.modal.element).modal('hide');
  };

  schat.ui.RoomTitleDialog = RoomTitleDialog;
})();