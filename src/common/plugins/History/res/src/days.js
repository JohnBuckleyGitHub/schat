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
  var prefix = '#day-' + day;
  if ($(prefix).length)
    return;

  var html = '<div class="day" id="day-' + day + '">' +
             '<div class="day-header expanded">' +
             '  <div class="day-day"><a href="#" data-day="' + day + '">' + SimpleChat.day(day) + '</a></div>' +
             '  <div class="day-close"><a class="close" href="#">&times;</a></div>' +
             '</div>' +
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

  if ($('.day-day').width() > 100)
    Utils.adjustWidth($('.day-day'));

  /*
   * Сворачивание или разворачивание блока сообщений.
   */
  $(prefix + ' .day-day a').on('click', function(event) {
    event.preventDefault();

    var header = $(prefix + ' .day-header');
    if (header.hasClass('expanded')) {
      header.removeClass('expanded');
      $(prefix + ' .day-body').hide();
    }
    else {
      header.addClass('expanded');
      $(prefix + ' .day-body').show();
    }

    alignChat();
  });

  /*
   * Удаление блока сообщений.
   */
  $(prefix + ' .day-close a').on('click', function(event) {
    event.preventDefault();
    $(prefix).remove();

    var index = Messages.days.indexOf(day);
    if (index != -1)
      Messages.days.splice(index, 1);

    alignChat();
  });

  /*
   * Показ кнопки закрытия.
   */
  $('#day-' + day + ' .day-day').on('mouseenter', function() {
    $('#day-' + day + ' .day-close').fadeIn('fast');
  });

  /*
   * Сокрытие кнопки закрытия.
   */
  $('#day-' + day + ' .day-header').on('mouseleave', function() {
    $('#day-' + day + ' .day-close').fadeOut('fast');
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


/*
 * Перевод дат при смене языка.
 */
Messages.retranslate = function() {
  $("[data-day]").each(function() {
    $(this).text(SimpleChat.day($(this).attr('data-day')));
  });

  Utils.adjustWidth($('.day-day'));
};

if (typeof ChatView !== 'undefined')
  ChatView.reload.connect(Messages.reload);

if (typeof SimpleChat !== 'undefined')
  SimpleChat.retranslated.connect(Messages.retranslate);
