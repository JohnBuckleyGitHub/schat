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

//Align our chat to the bottom of the window.  If true is passed, view will also be scrolled down
function alignChat() {
	var windowHeight = window.innerHeight;

	if (windowHeight > 0) {
		var contentElement = document.getElementById('Chat');
		var contentHeight = contentElement.offsetHeight;
		if (windowHeight - contentHeight > 0) {
			contentElement.style.position = 'relative';
			contentElement.style.top = (windowHeight - contentHeight) + 'px';
		} else {
			contentElement.style.position = 'static';
		}
	}

	scrollToBottom();
}

//Auto-scroll to bottom.  Use nearBottom to determine if a scrollToBottom is desired.
function nearBottom() {
	return ( document.body.scrollTop >= ( document.body.offsetHeight - ( window.innerHeight * 1.2 ) ) );
}

function scrollToBottom() {
	document.body.scrollTop = document.body.offsetHeight;
}

//Appending new content to the message view
function appendMessage(html) {
	$('#Chat').append(html);

	alignChat();
}

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

// Удаление статуса сообщения и времени доставки.
function setMessageState(id, classes, timestamp, seconds) {
	var prefix = id + ' > div.blocks';

	$(prefix).attr('class', 'blocks ' + classes);

	prefix += ' > .date-time-block > ';
	$(prefix + '.timestamp').text(timestamp);
	$(prefix + '.seconds').text(seconds);
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

