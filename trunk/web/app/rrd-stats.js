var exec    = require('child_process').exec;
var packets = require('./net/packets');
var cache   = require('./net/memory-cache');
var ChatId  = require('./id/cid').ChatId;
var core    = require('./core.js');

var child;
var records = {};
var last    = [0, 0];

'use strict';

var HOUR  = 3600;
var DAY   = 86400;
var WEEK  = 604800;
var MONTH = 2592000;
var YEAR  = 31536000;

function graph(seconds) {
  var suffix = '';

  if (seconds == HOUR)
    suffix = 'hour';
  else if (seconds == DAY)
    suffix = 'day';
  else if (seconds == WEEK)
    suffix = 'week';
  else if (seconds == MONTH)
    suffix = 'month';
  else if (seconds == YEAR)
    suffix = 'year';

  child = exec('rrdtool graph public/img/stats/users-' + suffix + '.png --end now --start end-' + seconds + 's ' +
      '--width 600 --height 250 --lower-limit 0 --slope-mode --watermark "Simple Chat" ' +
      'DEF:users=var/users.rrd:users:MAX AREA:users#47a447:"Users online" ' +
      'GPRINT:users:LAST:"Cur\\:%4.0lf" ' +
      'GPRINT:users:MIN:"Min\\:%4.0lf" ' +
      'GPRINT:users:AVERAGE:"Avg\\:%4.0lf" ' +
      'GPRINT:users:MAX:"Max\\:%4.0lf\\j" ' +
      'DEF:rooms=var/users.rrd:rooms:MAX LINE3:rooms#d2322d:"Rooms       " ' +
      'GPRINT:rooms:LAST:"Cur\\:%4.0lf" ' +
      'GPRINT:rooms:MIN:"Min\\:%4.0lf" ' +
      'GPRINT:rooms:AVERAGE:"Avg\\:%4.0lf" ' +
      'GPRINT:rooms:MAX:"Max\\:%4.0lf" ',
      { cwd:__dirname + '/../' }, function(error) {});
}


function write() {
  function callback() {
    graph(600);
    graph(HOUR);
    graph(DAY);
    graph(WEEK);
    graph(MONTH);
    graph(YEAR);
  }

  var text = '';
  for (var key in records) if (records.hasOwnProperty(key)) {
    text += ' ' + key + ':' + records[key][0] + ':' + records[key][1];
  }

  if (text === '') {
    exec('rrdtool update var/users.rrd N:' + last[0] + ':' + last[1], { cwd:__dirname + '/../' }, callback);
    return;
  }

  records = {};
  child = exec('rrdtool update var/users.rrd ' +  text, { cwd:__dirname + '/../' }, callback);
}


/**
 * Обновление данных.
 *
 * @param data
 * @param date
 */
function update(data, date) {
  date = Math.floor(date / 1000);
  if (!records.hasOwnProperty(date) || records[date][0] < data.users[0] || records[date][0] < data.rooms[0]) {
    last = [data.users[0], data.rooms[0]];
    records[date] = last;
  }
}


function close() {
  var date = Math.floor(new Date().getTime() / 1000);
  last = [0, 0];
  records[date] = last;
}


/**
 * Обработка авторизации на вышестоящем сервере.
 *
 * @param status
 */
function authReply(status) {
  if (status !== 200)
    return;

  function callback() {
    core.parent.get({request:core.serverId() + '/stats'}, function(reply) {
      if (reply.status == 200)
        update(reply.data[0], reply.date);
    });

    setInterval(write, 60000);
  }

  child = exec('rrdtool create var/users.rrd --step 1 --no-overwrite ' +
      'DS:users:GAUGE:86400:0:U ' +
      'DS:rooms:GAUGE:86400:0:U ' +
      'RRA:MAX:0.5:1:600 ' +
      'RRA:MAX:0.5:6:600 RRA:MIN:0.5:6:600 ' +
      'RRA:MAX:0.5:144:600 RRA:MIN:0.5:144:600 ' +
      'RRA:MAX:0.5:1008:600 RRA:MIN:0.5:1008:600 ' +
      'RRA:MAX:0.5:4320:600 RRA:MIN:0.5:4320:600 ',
      { cwd:__dirname + '/../' }, callback);
}

exports.init = function() {
  core.parent._socket.on('auth-reply', authReply);

  core.parent.on('packet.RES', function(data) {
    if (data.id === core.serverId() + '/stats')
      update(data.data[0], data.date);
  });

  core.parent._socket.on('close', close);
};
