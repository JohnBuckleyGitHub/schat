(function(){
  var Loader = {};

  Loader.widget = document.getElementById('loading');
  Loader.visible = true;

  Loader.show = function(timeout) {
    if (Loader.visible)
      return;

    timeout = timeout || 300;
    Loader.visible = true;

    Loader.timeout = setTimeout(function() {
      Loader.widget.style.display = 'block';
      setTimeout(function() { Loader.widget.style.opacity = 0.75; }, 0);
    }, timeout);
  };

  Loader.hide = function() {
    if (!Loader.visible)
      return;

    clearTimeout(Loader.timeout);
    Loader.visible = false;
    Loader.widget.style.opacity = 0;
    setTimeout(function() { Loader.widget.style.display = 'none'; }, 500);
  };

  window.schat.ui.Loader = Loader;
})();