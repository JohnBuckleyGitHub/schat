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
	
	$('#paths').click(function() {
		toggle('#paths');
	});
	
	$('#other').click(function() {
		toggle('#other');
	});
	
	$('#about').fadeIn();
	
	function toggle(prefix) {
		var paths = $(prefix);
		var pathsBody = $(prefix + '-body');
		
		if (pathsBody.is(':hidden')) {
			pathsBody.fadeIn();
			paths.removeClass('toggle-expand');
			paths.addClass('toggle');
		}
		else {
			pathsBody.fadeOut();
			paths.removeClass('toggle');
			paths.addClass('toggle-expand');
		}
	}

});
