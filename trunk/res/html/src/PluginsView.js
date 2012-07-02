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

var Plugins = {
  add: function(data)
  {
    var html = '<div class="plugin-wrapper" id="' + data.id + '">' +
                  '<div class="plugin" style="background-image:url(' + data.icon + ');">' +
                    '<div class="plugin-details">' +
                      '<div class="plugin-header"><span class="plugin-title">' + data.title + '</span><span class="plugin-version">' + data.version + '</span></div>' +
                      '<div class="desc-description">' + data.desc + '</div>' +
                    '</div>' +
                    '<div class="plugin-controls">' +
                      '<div class="checkbox enable-checkbox">' +
                        '<label>' +
                          '<input type="checkbox"> ' +
                          '<span class="enable-checkbox-text">' +
                            '<span class="enabled-text" data-tr="enabled">' + Plugins.tr('enabled') + '</span>' +
                            '<span class="enable-text" data-tr="enable">' + Plugins.tr('enable') + '</span>' +
                          '</span>' +
                        '</label>' +
                      '</div>' +
                    '</div>' +
                  '</div>' +
                '</div>';

    $('#plugins-list').append(html);

    Plugins.enable(data.id, data.enabled);

    $('#' + data.id + ' input:checkbox').click(function() {
      Plugins.enable(data.id, $('#' + data.id + ' input:checkbox').prop('checked'));
    });
  },


  enable: function(id, enable)
  {
    if (enable) {
      $('#' + id + ' input:checkbox').attr('checked', 'checked');
      $('#' + id).addClass('plugin-enabled');
      $('#' + id + ' .enabled-text').show();
      $('#' + id + ' .enable-text').hide();
    }
    else {
      $('#' + id + ' input:checkbox').removeAttr('checked');
      $('#' + id).removeClass('plugin-enabled');
      $('#' + id + ' .enabled-text').hide();
      $('#' + id + ' .enable-text').show();
    }
  },

  retranslate: function() {
    $("[data-tr]").each(function() {
      Plugins.TR($(this).attr("data-tr"));
    });
  },

  tr: function(key) {
    return SimpleChat.translate(key);
  },

  TR: function(key) {
    $("[data-tr='" + key + "']").html(Plugins.tr(key));
  }
};

$(document).ready(function() {
  var plugins = PluginsView.list();
  for (var i = 0; i < plugins.length; i++) {
    Plugins.add(plugins[i]);
  }
});

if (typeof PluginsView === "undefined") {
  PluginsView = {
    list: function() { return []; }
  };
}
else {
  SimpleChat.retranslated.connect(Plugins.retranslate);
}
