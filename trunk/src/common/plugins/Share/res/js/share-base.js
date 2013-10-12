(function() {
  'use strict';

  var progress = {};

  function uploadStarted(roomId, oid) {
    if (Settings.getId() !== roomId)
      return;

    var message = new schat.ui.ImageUpload(oid);
    Messages.addRawMessage(message.cont);

    progress[oid] = message;

    message.cancelBtn.addEventListener('click', function() {
      Share.cancel(oid);
      message.cont.parentNode.removeChild(message.cont);
      delete progress[oid];

      alignChat();
    });

    Messages.added.length = 0;
    alignChat();
  }


  function uploadProgress(roomId, oid, bytesSent, bytesTotal) {
    if (Settings.getId() !== roomId || !progress.hasOwnProperty(oid))
      return;

    progress[oid].setProgress(bytesSent, bytesTotal);
  }


  function uploadStatus(roomId, oid, status) {
    if (Settings.getId() !== roomId || !progress.hasOwnProperty(oid))
      return;

    progress[oid].setStatus(status);
  }


  Messages.addImageMessage = function(json) {
    var oid = json.OID;
    if (progress.hasOwnProperty(oid)) {
      progress[oid].cont.parentNode.removeChild(progress[oid].cont);
      delete progress[oid];
    }

    var message = new schat.ui.ImageMessage(json);

    Messages.addHintedRawMessage(message.cont, json.Hint);
    ChatView.setLastMessage(json.Date);
  };


  Messages.imageRemover = function(cont) {
    cont.children[0].setAttribute('class', 'panel-heading removed');

    var body = cont.children[1];
    body.setAttribute('class', 'panel-body removed');
    body.innerHTML = '<img class="img-thumbnail" src="qrc:/images/Share/removed-image.png">';
  };


  Modal.create.image = function(event)
  {
    schat.ui.modal.current = new schat.ui.ImageDialog(event.target.src.replace('/S/', '/L/'));
  };


  if (typeof Share !== 'undefined') {
    Share.uploadStarted.connect(uploadStarted);
    Share.uploadProgress.connect(uploadProgress);
    Share.uploadStatus.connect(uploadStatus);
  }
  else {
    window.Share = {}
  }

})();