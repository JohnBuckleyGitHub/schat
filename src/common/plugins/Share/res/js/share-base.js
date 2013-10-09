(function() {
  'use strict';

  var progress = {};

  function uploadStarted(roomId, oid) {
    if (Settings.getId() !== roomId)
      return;

    var message = new window.schat.ui.ImageMessage(oid);
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


  if (typeof Share !== 'undefined') {
    Share.uploadStarted.connect(uploadStarted);
    Share.uploadProgress.connect(uploadProgress);
    Share.uploadStatus.connect(uploadStatus);
  }
  else {
    window.Share = {}
  }

})();