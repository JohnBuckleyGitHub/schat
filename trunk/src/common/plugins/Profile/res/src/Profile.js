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

Profile.Field.city = function(key, value) {
  Profile.addRow(key, Utils.left(htmlspecialchars(value), 128));
};

Profile.Field.site = function(key, value) {
  Profile.addRow(key, Utils.left(htmlspecialchars(value), 128));
};

Profile.Field.email = function(key, value) {
  var addr = Utils.left(htmlspecialchars(value), 254).replace(/\s+/gi, '');
  if (addr.indexOf("mailto:") == 0)
    addr = addr.slice(7);

  Profile.addRow(key, '<a href="mailto:' + addr + '">' + addr + '</a>');
};
