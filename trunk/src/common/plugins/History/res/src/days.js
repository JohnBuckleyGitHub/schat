/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright (c) 2008-2012 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

Messages.days = [];

/*
 * Создание при необходимости блока с сообщениями за 1 день.
 */
Messages.day = function(day) {
  if ($('#day-' + day).length)
    return;

  var html = '<div class="day" id="day-' + day + '">' +
             '<div class="day-header"><div class="day-day"><a href="#">' + SimpleChat.day(day) + '</a></div></div>' +
             '<div class="day-body"></div></div>';

  if (Messages.days.indexOf(day) == -1) {
    Messages.days.push(day);
    Messages.days.sort();
  }

  var index = Messages.days.indexOf(day);
  if (index == Messages.days.length - 1)
    $('#Chat').append(html);
  else
    $('#day-' + Messages.days[index + 1]).before(html);

  var d = $('#day-' + day);
  d.data('state', 'open');

  $('#day-' + day + ' .day-header a').on('click', function(event) {
    event.preventDefault();

    if (d.data('state') == 'open') {
      d.data('state', 'closed');
      $('#day-' + day + ' .day-body').hide();
    }
    else {
      d.data('state', 'open');
      $('#day-' + day + ' .day-body').show();
    }

    alignChat();
  });
};


/*
 * Добавление сырого сообщения.
 *
 * \param html HTML текст сообщения.
 * \param id   Идентификатор сообщения.
 * \param day  День сообщения.
 */
Messages.addRawMessage = function(html, id, day) {
  $('#day-' + day + ' .day-body').append(html);
  Messages.add(id);
};


/*
 * Очистка страницы.
 */
Messages.reload = function()
{
  if (Pages.current != 0)
    return;

  $('.day').remove();
  Messages.days = [];
  alignChat();
};
