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
  },

  toggle: function() {
    var body = $('#' + $(this).attr('id') + '-body');
    if (body.is(':hidden')) {
      body.show();
      $(this).removeClass('toggle-expand');
      $(this).addClass('toggle');
    }
    else {
      body.hide();
      $(this).removeClass('toggle');
      $(this).addClass('toggle-expand');
    }
  }
};


$(document).ready(function() {

  $('.header').click(AboutUtils.toggle);

  AboutUtils.retranslate();
  $('#version').html('<a href="http://wiki.schat.me/Simple_Chat_' + About.version('app') + '">' + About.version('app') + '</a>');
  $('#qt-version').text(About.version('qt'));
  $('#webkit-version').text(About.version('webkit'));
  $('#preferences').html(About.path('preferences'));
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
