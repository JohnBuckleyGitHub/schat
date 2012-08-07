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

var AboutUtils = {
  retranslate: function() {
    $("[data-tr]").each(function() {
      AboutUtils.TR($(this).attr("data-tr"));
    });
  },

  tr: function(key) {
    return SimpleChat.translate(key);
  },

  TR: function(key) {
    $("[data-tr='" + key + "']").html(AboutUtils.tr(key));
  }
};


$(document).ready(function() {

  $('#paths').click(function() {
    toggle('#paths');
  });

  $('#other').click(function() {
    toggle('#other');
  });

  AboutUtils.retranslate();
  $('#version').html('<a href="http://wiki.schat.me/Simple_Chat_' + About.version('app') + '">' + About.version('app') + '</a>');
  $('#qt-version').text(About.version('qt'));
  $('#webkit-version').text(About.version('webkit'));
  $('#preferences').html(About.path('preferences'));

  function toggle(prefix) {
    var paths = $(prefix);
    var pathsBody = $(prefix + '-body');

    if (pathsBody.is(':hidden')) {
      pathsBody.show();
      paths.removeClass('toggle-expand');
      paths.addClass('toggle');
    }
    else {
      pathsBody.hide();
      paths.removeClass('toggle');
      paths.addClass('toggle-expand');
    }
  }
});

if (typeof About === "undefined") {
  About = {
    path: function(type) { return type; },
    version: function(type) { return type; }
  };
}

if (typeof SimpleChat === "undefined") {
  SimpleChat = {
    translate: function(key) { return key; }
  };
}
else {
  SimpleChat.retranslated.connect(AboutUtils.retranslate);
}
