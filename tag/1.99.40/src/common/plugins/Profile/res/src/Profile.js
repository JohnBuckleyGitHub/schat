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

Profile.Field.country = function(key, value) {
  if (value.length != 2)
    return;

  var name = Utils.tr("country-" + value);
  if (name == "country-" + value)
    return;

  Profile.addRow(key, '<i class="flag-' + value + '"></i> <span data-tr="country-' + value + '">' + name + '</span>');
};

Profile.Field.city = Profile.Field.name;

Profile.Field.site = function(key, value) {
  var addr = Utils.left(htmlspecialchars(value.replace(/\s+/gi, '')), 254);
  var title = addr;
  if (addr.indexOf("http://") == 0)
    title = addr.slice(7);
  else if (addr.indexOf("https://") == 0)
    title = addr.slice(8);
  else
    addr = "http://" + addr;

  Profile.addRow(key, '<a href="' + addr + '">' + title + '</a>');
};


Profile.Field.email = function(key, value) {
  var addr = Utils.left(htmlspecialchars(value.replace(/\s+/gi, '')), 254);
  if (addr.indexOf("mailto:") == 0)
    addr = addr.slice(7);

  Profile.addRow(key, '<a href="mailto:' + addr + '">' + addr + '</a>');
};


var ProfilePlugin = {
  process: function(key, json) {
    var id = "#" + key;
    if (!json.hasOwnProperty("geo") || json.geo.country == "") {
      $(id + ' [class^="flag-"]').remove();
      return;
    }

    var country = json.geo.country.toLowerCase();
    if (country.length != 2)
      return;

    $(id).append(' <span><i class="flag-' + country + '" data-original-title="' + Utils.tr('country-' + country) + '"></i></span>');
    $(id + ' .flag-' + country).tooltip();
  },


  connection: function(json) {
    if (!json.hasOwnProperty('geo') || json.geo.country === '')
      return;

    var country = json.geo.country.toLowerCase();
    if (country.length != 2)
      return;

    $('#modal-body').append(Utils.row('field-country', '<i class="flag-' + country + '"></i> '
      + '<span data-tr="country-' + country + '">' + Utils.tr('country-' + country) + '</span>'));

    if (json.geo.org != '') {
      var org = json.geo.org;
      var index = org.indexOf(' ');
        if (index == -1)
          return;

      $('#modal-body').append(Utils.row('field-isp', Utils.left(org.slice(index), 100)));
    }
  }
};

UserHooks.process.push(ProfilePlugin.process);
UserHooks.connection.push(ProfilePlugin.connection);