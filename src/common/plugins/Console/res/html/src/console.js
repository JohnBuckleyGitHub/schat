
var Console = {
  current: 'try'
};


/*
 * Чтение данных фидов.
 */
Console.feed = {
  /*
   * Чтение новых данных.
   */
  read: function(json) {
    console.log(json);

    if (json.type == 'reply') {
      var path = json.name.split('/');
      if (path.length < 2 || path[0] != 'console')
        return;

      var key = path[1];
      if (key == 'try')
        key = 'tryAccess';

      if (json.cmd == 'get' && Console.feed.get.hasOwnProperty(key))
        Console.feed.get[key](json);
    }
  },


  /*
   * Чтение ответов на "get" запросы.
   */
  get: {}
};


/*
 * Чтение ответа на "get" запрос "console/try".
 */
Console.feed.get.tryAccess = function(json) {
  Loader.spinner.remove('loading/try');
  var page = $('#page');

  if (json.status == 403) {
    page.html('<div class="alert alert-error" data-tr="console_bad_server">' + Utils.tr('console_bad_server') + '</div>');
  }
  else {
    page.html('<div class="alert alert-error"><strong>' + json.status + '</strong> ' + SimpleChat.statusText(json.status) + '</div>');
  }
};


$(document).ready(function() {
  Loader.spinner.add('loading/try');

  SimpleChat.get(SimpleChat.serverId(), 'console/try');
});


if (typeof ConsoleView === 'undefined') {
  ConsoleView = {}
}
else {
  ConsoleView.feed.connect(Console.feed.read);
}
