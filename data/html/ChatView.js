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
	$.fx.off = true;

	$(window).resize(function() {
		alignChat(true);
	});
	
	$('a.nick').click(nickClicked);

	alignChat(true);
});

//Align our chat to the bottom of the window.  If true is passed, view will also be scrolled down
function alignChat(shouldScroll) {
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
	
	if (shouldScroll)
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
	var shouldScroll = nearBottom();
	$('#Chat').append(html);
	$('a.nick').unbind('click', nickClicked);
	$('a.nick').bind('click', nickClicked);
	alignChat(shouldScroll);
}

function showSeconds(show) {
	show ? $('.seconds').show() : $('.seconds').hide();
}


function nickClicked() {
	try {
		ChatView.nickClicked($(this).attr('class'));
	} catch (e) { alert(e); }
	event.preventDefault();
}
