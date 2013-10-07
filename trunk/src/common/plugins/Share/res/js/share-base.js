(function() {
  'use strict';

  var progress = {};

  function uploadStarted(roomId, oid) {
    if (Settings.getId() !== roomId)
      return;

    var message = new window.schat.ui.ImageMessage(oid);
    Messages.addRawMessage(message.cont);

    progress[oid] = message;

    $(message.cancelBtn).on('click.cancel', function() {
      Share.cancel(oid);
      message.cont.parentNode.removeChild(message.cont);
      delete progress[oid];

      alignChat();
    });

    alignChat();
  }


  function uploadProgress(roomId, oid, bytesSent, bytesTotal) {
    if (Settings.getId() !== roomId || !progress.hasOwnProperty(oid))
      return;

    progress[oid].setProgress(bytesSent, bytesTotal);
    console.log(roomId, oid, bytesSent, bytesTotal);
  }


  if (typeof Share !== 'undefined') {
    Share.uploadStarted.connect(uploadStarted);
    Share.uploadProgress.connect(uploadProgress);
  }
  else {
    window.Share = {}
  }

})();