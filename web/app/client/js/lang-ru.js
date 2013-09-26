(function() {
  'use strict';

  function numpf(n, f, s, t) {
    // f - 1, 21, 31, ...
    // s - 2-4, 22-24, 32-34 ...
    // t - 5-20, 25-30, ...
    var n10 = n % 10;
    if ( (n10 == 1) && ( (n == 1) || (n > 20) ) ) {
      return f;
    } else if ( (n10 > 1) && (n10 < 5) && ( (n > 20) || (n < 10) ) ) {
      return s;
    } else {
      return t;
    }
  }

  var l = {};
  l['common-signin']     = 'Войти';
  l['common-nick']       = 'Ник';
  l['common-your-nick']  = 'Ваш ник';
  l['common-email']      = 'Email';
  l['common-your-email'] = 'Ваш Email';
  l['common-male']       = 'Мужской';
  l['common-female']     = 'Женский';
  l['common-rooms']      = 'Комнаты';
  l['common-stats']      = 'Статистика';
  l['common-hour']       = 'Час';
  l['common-day']        = 'День';
  l['common-week']       = 'Неделя';
  l['common-month']      = 'Месяц';
  l['common-year']       = 'Год';

  l['oauth-header']      = 'Войти в чат через свой аккаунт в';
  l['channels-no-title'] = 'Без заголовка';

  l['stats-stats']       = 'Сейчас {0} онлайн в {1}. Пиковый онлайн <b>{2}</b> был <span class="timeago" data-date="{3}"></span>.';
  l['stats-users']       = function(param) { return numpf(param, "<b>%d</b> пользователь", "<b>%d</b> пользователя", "<b>%d</b> пользователей"); };
  l['stats-rooms']       = function(param) { return numpf(param, "<b>%d</b> комнате", "<b>%d</b> комнатах", "<b>%d</b> комнатах"); };
  l['stats-title']       = 'Пользователи онлайн';
  l['stats-title-hour']  = 'за час';
  l['stats-title-day']   = 'за день';
  l['stats-title-week']  = 'за неделю';
  l['stats-title-month'] = 'за месяц';
  l['stats-title-year']  = 'за год';

  l['user-not-found']      = 'Пользователь не найден';
  l['user-status-offline'] = 'Не в сети';
  l['user-status-online']  = 'В сети';
  l['user-status-away']    = 'Отсутствую';
  l['user-status-dnd']     = 'Не беспокоить';
  l['user-status-free']    = 'Готов общаться';

  l.timeago = {
    prefixAgo: null,
    prefixFromNow: "через",
    suffixAgo: "назад",
    suffixFromNow: null,
    seconds: "меньше минуты",
    minute: "минуту",
    minutes: function(value) { return numpf(value, "<b>%d<b/> минуту", "<b>%d</b> минуты", "<b>%d</b> минут"); },
    hour: "час",
    hours: function(value) { return numpf(value, "<b>%d</b> час", "<b>%d</b> часа", "<b>%d</b> часов"); },
    day: "день",
    days: function(value) { return numpf(value, "<b>%d</b> день", "<b>%d</b> дня", "<b>%d</b> дней"); },
    month: "месяц",
    months: function(value) { return numpf(value, "<b>%d</b> месяц", "<b>%d</b> месяца", "<b>%d</b> месяцев"); },
    year: "год",
    years: function(value) { return numpf(value, "<b>%d</b> год", "<b>%d</b> года", "<b>%d</b> лет"); }
  };

  schat.lang.ru = l;
})();