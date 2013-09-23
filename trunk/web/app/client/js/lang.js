(function() {
  'use strict';

  schat.lang.current = localStorage.getItem('language') || (navigator.language || navigator.userLanguage).substr(0, 2);

  window.tr = function(key, param) {
    var lang = schat.lang.current;
    if (!schat.lang.hasOwnProperty(lang)) {
      lang = 'en';

      if (!schat.lang.hasOwnProperty(lang))
        return key;
    }

    var value = schat.lang[lang][key];
    if (!value && lang != 'en')
      value = schat.lang.en[key];

    value = value || key;
    if (schat.utils.isFunction(value))
      return value(param).replace('%d', param);

    return value;
  };
})();