
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
    if (json.type == 'reply') {
      if (!json.path.length)
        return;

      var path = json.path;
      if (path == 'try')
        path = 'tryAccess';

      try {
        Console.feed[json.feed][json.cmd][path](json);
      }
      catch (e) {}
    }
  },


  /*
   * Чтение ответов на "get", "put", "post" и "delete" запросы.
   */
  console: {
    get: {},
    put: {},
    post: {},
    del: {}
  },


  storage: {
    get: {},
    put: {},
    post: {},
    del: {}
  }
};


/*
 * Чтение ответа на "get" запрос "console/try".
 */
Console.feed.console.get.tryAccess = function(json) {
  Loader.spinner.remove('loading/try');
  var page = $('#page');

  if (json.status == 402) {
    Console.login.load();
  }
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
  ConsoleView = {
    toPassword: function(password) { return password; }
  }

}
else {
  ConsoleView.feed.connect(Console.feed.read);
}
