(function() {
  'use strict';

  function aclValue(id) {
    var acl = SimpleChat.match(Settings.getId(), id);

    if (acl & 9)
      return ACL_OWNER;

    var feed = SimpleChat.feed(Settings.getId(), FEED_NAME_ACL, 3);
    if (feed === false || !feed.hasOwnProperty(id))
      return -1;

    if (acl & 16)
      return ACL_MODERATOR;

    else if (acl == ACL_READWRITE || acl == ACL_READONLY || acl == ACL_FORBIDDEN)
      return acl;

    return -1;
  }


  var title  = schat.ui.modal.title;
  var footer = schat.ui.modal.footer;
  var body   = schat.ui.modal.body;
  var create = window.schat.ui.createElement;

  function RoomAclDialog(id) {
    title.innerHTML = Messages.nameTemplate(SimpleChat.channel(id));
    title.removeAttribute('data-tr');

    body.innerHTML = '<form>' +
                       '<div id="acl-row" class="form-group">' +
                         '<label for="acl" data-tr="channels-permissions" class="options-label control-label">' + tr('channels-permissions') + '</label> ' +
                         '<select id="acl" data-user="' + id + '" class="btn btn-default">' +
                           '<option value="-1" data-tr="channels-default">'   + tr('channels-default')   + '</option>' +
                           '<option value="15" data-tr="channels-owner">'     + tr('channels-owner')     + '</option>' +
                           '<option value="22" data-tr="channels-moderator">' + tr('channels-moderator') + '</option>' +
                           '<option value="6" data-tr="channels-readwrite">'  + tr('channels-readwrite') + '</option>' +
                           '<option value="4" data-tr="channels-readonly">'   + tr('channels-readonly')  + '</option>' +
                           '<option value="0" data-tr="channels-forbidden">'  + tr('channels-forbidden') + '</option>' +
                         '</select> ' +
                         '<i id="acl-spinner" class="icon-spinner hide"></i>' +
                         '<div id="acl-error" class="alert alert-error hide"></div>' +
                       '</div>' +
                    '</form>';

    this.acl = document.getElementById('acl');
    this.acl.value = aclValue(id);

    this.btn = footer.appendChild(create('button', {type:'button', class:'btn btn-primary', 'data-tr':'ok'}));
    footer.appendChild(create('button', {type:'button', class:'btn btn-default', 'data-dismiss':'modal', 'data-tr':'cancel'}));

    var that = this;
    this.btn.addEventListener('click', function() { that.setAcl.apply(that, arguments); });
  }


  RoomAclDialog.prototype.setAcl = function(event) {
    event.preventDefault();

    var value = this.acl.value;
    var user  = this.acl.getAttribute('data-user');
    var id    = Settings.getId();

    if (value == ACL_OWNER)
      SimpleChat.request(id, FEED_METHOD_POST,   ACL_FEED_HEAD_OWNER_REQ, {'value':user,'options':6});
    else if (value == ACL_DEFAULT)
      SimpleChat.request(id, FEED_METHOD_DELETE, ACL_FEED_HEAD_OTHER_REQ + '/' + user, {'options':6});
    else
      SimpleChat.request(id, FEED_METHOD_POST,   ACL_FEED_HEAD_OTHER_REQ + '/' + user, {'value':value,'options':6});

    $(schat.ui.modal.element).modal('hide');
  };

  schat.ui.RoomAclDialog = RoomAclDialog;
})();