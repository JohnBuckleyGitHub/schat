$(document).ready(function() {
  schat.net.client.start();

  $('body').on('click', '.internal', function(event) {
    event.preventDefault();

    schat.router.navigate($(this).attr('href'));
  });

  schat.router.checkRoutes(document.location.pathname);
});