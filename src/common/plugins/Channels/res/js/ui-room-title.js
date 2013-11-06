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

    var form = body.appendChild(create('form', {class:'form-horizontal'}));
    form.innerHTML =
        '<div class="form-group">' +
          '<div class="col-sm-12">' +
            '<input type="text" class="form-control" id="title-edit" maxlength="200" autofocus="autofocus">' +
          '</div>' +
        '</div>';

    this.btn = footer.appendChild(create('button', {type:'button', class:'btn btn-primary', 'data-tr':'ok'}));
    footer.appendChild(create('button', {type:'button', class:'btn btn-default', 'data-dismiss':'modal', 'data-tr':'cancel'}));

    this.titleEdit = document.getElementById('title-edit');
    this.titleEdit.value = document.getElementById('channel-title-text').textContent;

    var that = this;

    form.addEventListener('submit',    function() { that.setTitle.apply(that, arguments); });
    this.btn.addEventListener('click', function() { that.setTitle.apply(that, arguments); });
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