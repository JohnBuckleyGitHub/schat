(function() {
  'use strict';

  var l = {};
  l['common-signin']     = 'Sign in';
  l['common-nick']       = 'Nick';
  l['common-your-nick']  = 'Your Nick';
  l['common-email']      = 'Email';
  l['common-your-email'] = 'Your Email';
  l['common-male']       = 'Male';
  l['common-female']     = 'Female';
  l['common-rooms']      = 'Rooms';
  l['common-stats']      = 'Statistics';
  l['common-hour']       = 'Hour';
  l['common-day']        = 'Day';
  l['common-week']       = 'Week';
  l['common-month']      = 'Month';
  l['common-year']       = 'Year';

  l['oauth-header']      = 'Sign in using your account with';
  l['channels-no-title'] = 'No title';

  l['stats-stats']       = 'Now {0} online in {1}. Peak online <b>{2}</b> was <span class="timeago" data-date="{3}"></span>.';
  l['stats-users']       = function(param) { return (param === 1 ? '<b>%d</b> user' : '<b>%d</b> users'); };
  l['stats-rooms']       = function(param) { return (param === 1 ? '<b>%d</b> room' : '<b>%d</b> rooms'); };
  l['stats-title']       = 'Users online';
  l['stats-title-hour']  = 'by hour';
  l['stats-title-day']   = 'by day';
  l['stats-title-week']  = 'by week';
  l['stats-title-month'] = 'by month';
  l['stats-title-year']  = 'by year';

  l['user-not-found']      = 'User not found';
  l['user-status-offline'] = 'Offline';
  l['user-status-online']  = 'Online';
  l['user-status-away']    = 'Away';
  l['user-status-dnd']     = 'Do not disturb';
  l['user-status-free']    = 'Free for Chat';

  l.timeago = {
    prefixAgo: null,
    prefixFromNow: null,
    suffixAgo: "ago",
    suffixFromNow: "from now",
    seconds: "less than a minute",
    minute: "about a minute",
    minutes: "<b>%d</b> minutes",
    hour: "about an hour",
    hours: "about <b>%d</b> hours",
    day: "a day",
    days: "<b>%d</b> days",
    month: "about a month",
    months: "<b>%d</b> months",
    year: "about a year",
    years: "<b>%d</b> years",
    wordSeparator: " ",
    numbers: []
  };

  schat.lang.en = l;
})();