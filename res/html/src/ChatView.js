/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright (c) 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

$(document).ready(function() {
	//$.fx.off = true;

	$(window).resize(function() {
		alignChat();
	});

	var timeoutID;
	$('#topic-wrapper').hover(
		function () {
			window.clearTimeout(timeoutID);
			$('#body').removeClass("no-topic-author");
		},
		function () {
			timeoutID = window.setTimeout(function() {
				$('#body').addClass("no-topic-author");
			}, 2000);
		}
	);

	alignChat();
});


function alignChat() {
	var windowHeight = window.innerHeight;

	if (windowHeight > 0) {
		var contentElement = document.getElementById('Chat');
		var contentHeight = contentElement.offsetHeight;
		if (windowHeight - contentHeight > 0) {
			contentElement.style.position = 'relative';
			contentElement.style.top = (windowHeight - contentHeight - 4) + 'px';
		} else {
			contentElement.style.position = 'static';
		}
	}

	document.body.scrollTop = document.body.offsetHeight;
}


/// Добавление сообщения.
/// Эта функция вызывается из C++ кода.
function addMessage(data)
{
	var json = JSON.parse(data);

	if (json.Type == 'channel')
		addChannelMessage(json);
	else if (json.Type == 'service')
		addServiceMessage(json);
}


function addRawMessage(html) {
	$('#Chat').append(html);

	alignChat();
}


/// Добавление сообщения пользователя.
function addChannelMessage(json)
{
	if ($('#' + json.Id).length) {
		updateChannelMessage(json);
		return;
	}

	var html = '<div class="container ' + json.Type + '-type" id="' + json.Id + '">';
	html += '<div class="blocks ';
	html += json.Direction;

	if (json.Status !== undefined)
		html += ' ' + json.Status;

	if (json.Command == "me")
		html += ' me-action';

	html += '">';

	html += dateTemplate(json.Date, json.Day);
	html += nameTemplate(json);
	html += '<span class="msg-body-block">' + json.Text + '</span>';

	html += '</div>';
	html += '</div>';

	addRawMessage(html);
}


function addServiceMessage(json)
{
	var html = '<div class="container ' + json.Type + '-type" id="' + json.Id + '">';
	html += '<div class="blocks">';

	html += dateTemplate(json.Date, false);
	html += nameTemplate(json);
	html += '<span class="msg-body-block">' + json.Text + '</span>';

	html += '</div>';
	html += '</div>';

	addRawMessage(html);
}


/// Обновление информации сообщения.
function updateChannelMessage(json)
{
	var id = '#' + json.Id + ' > div.blocks';
	var classes = json.Direction;

	if (json.Status !== undefined)
		classes += ' ' + json.Status;

		if (json.Command == "me")
		classes += ' me-action';

	$(id).attr('class', 'blocks ' + classes);

	if (json.Date > 0) {
		var date = new Date(json.Date);
		if (json.Day === true)
			$(id + ' > .date-time-block > .day').text(dateDay(date));

		$(id + ' > .date-time-block > .time').text(dateTime(date));
		$(id + ' > .date-time-block > .seconds').text(dateSeconds(date));
	}
}


/// Возвращает HTML шаблон времени сообщения.
function dateTemplate(milliseconds, day)
{
	var out = '';

	if (milliseconds > 0) {
		var date = new Date(milliseconds);
		out += '<span class="date-time-block">';
		if (day === true) {
			out += '<span class="day">' + dateDay(date) + '</span> ';
		}
		out += '<span class="time">' + dateTime(date) + '</span>';
		out += '<span class="seconds">' + dateSeconds(date) + '</span> ';
		out += '</span>';
	}

	return out;
}


/// Возвращает день в формате день:месяц:год.
function dateDay(date)
{
	return datePad(date.getDate()) + ':' + datePad(date.getMonth() + 1) + ':' + date.getFullYear();
}


/// Возвращает время в формате часы:минуты.
function dateTime(date)
{
	return datePad(date.getHours()) + ':' + datePad(date.getMinutes());
}


/// Возвращает секунды в формате :секунды.
function dateSeconds(date)
{
	return ':' + datePad(date.getSeconds());
}


/// Дополняет число ведущим 0 при необходимости.
function datePad(n)
{
	return n < 10 ? '0' + n : n
}


/// Возвращает HTML шаблон имени канала.
function nameTemplate(json)
{
	var out = '';
	if (json.Author === undefined)
		return out;

	out += '<span class="nick-block"><a class="nick ';
	out += json.Author.Id + '" href="';
	out += json.Author.Url + '">';
	out += json.Author.Name;
	out += '</a></span> ';

	return out;
}


/// Обновление имени канала.
function updateChannelName(data)
{
	var json = JSON.parse(data);

	var a = $('a.' + json.Id);
	a.attr('href', json.Url);
	a.html(json.Name);
}


/// Включает или выключает отображение секунд.
function showSeconds(show) {
	if (show)
		$('#body').removeClass('no-seconds');
	else
		$('#body').addClass('no-seconds');
}


function showService(show) {
	if (show)
		$('#body').removeClass('no-service');
	else
		$('#body').addClass('no-service');

	alignChat();
}

function showTopic(show) {
	if (show)
		$('#topic-wrapper').show();
	else
		$('#topic-wrapper').hide();
}

// Повторно устанавливает обработчик кнопок.
function rebindButtonHandlers() {
	$('button').unbind('click', handleButton);
	$('button').bind('click', handleButton);
}

// Обработчик нажатия кнопки.
function handleButton() {
	var button = $(event.target);
	var data = button.attr('id').split(';');
	var id = data[0];
	var type = data[1];

	if (type == 'yes' || type == 'no') {
		$('#' + id).fadeOut('fast', function() {
			alignChat();
		});
	}

	try {
		SimpleChat.click(id, type);
	} catch (e) {}
}

function setTopic(topic)
{
	$('#topic-wrapper').html(topic);

	if (topic === '')
		$('#topic-wrapper').hide();
	else
		$('#topic-wrapper').show();
}

function updateUserNick(id, url, nick)
{
	var a = $('a.' + id);
	a.attr('href', url);
	a.html(nick);
}
