(function() {
  'use strict';

  function tr(key) {
    return SimpleChat.translate(key);
  }

  function TR(key) {
    $('[data-tr="' + key + '"]').html(tr(key));
  }

  function retranslate() {
    $('[data-tr]').each(function() {
      TR($(this).attr('data-tr'));
    });
  }

  if (typeof SimpleChat !== 'undefined') {
    SimpleChat.retranslated.connect(function() { retranslate(); });
  }

  window.tr              = tr;
  window.TR              = TR;
  schat.lang.retranslate = retranslate;

  // backward compatibility.
  window.Utils = window.Utils || {};
  window.Utils.tr          = tr;
  window.Utils.TR          = TR;
  window.Utils.retranslate = retranslate;
})();