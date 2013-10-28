(function() {
  'use strict';

  var progress = {};

  function uploadAdded(data) {
    console.log(data);

    if (typeof data !== 'object' || Settings.getId() !== data.roomId)
      return;

    Share.add(data);
  }


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


  /**
   * Добавление сообщения с типом image.
   *
   * @param json
   */
  Messages.addImageMessage = function(json) {
    var oid = json.OID;
    if (progress.hasOwnProperty(oid)) {
      if (json.Status === 200)
        progress[oid].cont.parentNode.removeChild(progress[oid].cont);
      else
        progress[oid].setStatus(json.Status);

      delete progress[oid];
    }

    if (document.getElementById(oid) !== null)
      return;

    var message = new schat.ui.ImageMessage(json);

    Messages.addHintedRawMessage(message.cont, json.Hint);
    ChatView.setLastMessage(json.Date);
  };


  Messages.imageRemover = function(cont) {
    cont.children[0].setAttribute('class', 'panel-heading removed');

    var body = cont.children[1];
    body.setAttribute('class', 'panel-body removed');
    body.innerHTML = '<img class="img-thumbnail" draggable="false" src="qrc:/images/Share/removed-image.png">';
  };


  Modal.create.image = function(event)
  {
    schat.ui.modal.current = new schat.ui.ImageDialog(event.target.href.replace('/O/', '/L/'));
  };


  if (typeof Share !== 'undefined') {
    Share.uploadAdded.connect(uploadAdded);
    Share.uploadStarted.connect(uploadStarted);
    Share.uploadProgress.connect(uploadProgress);
    Share.uploadStatus.connect(uploadStatus);
  }
  else {
    window.Share = {}
  }

})();