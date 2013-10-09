module.exports = function(grunt) {
  'use strict';

  grunt.initConfig({
    pkg: {
      rooms: 'src/common/plugins/Channels/res',
      share: 'src/common/plugins/Share/res'
    },
    concat: {
      js: {
        files: {
          '<%= pkg.rooms %>/js/rooms.js': [
            'web/app/3rdparty/bootstrap/js/modal.js',
            'web/app/client/js/3rdparty/jquery.timeago.js',
            'web/app/client/js/base.js',
            'web/app/client/js/utils.js',
            'web/app/client/js/phpjs.js',
            'res/js/lang.js',
            'web/app/client/js/ui/ui-base.js',
            'web/app/client/js/ui/ui-modal.js',
            'web/app/client/js/ui/ui-index.js',
            '<%= pkg.rooms %>/js/rooms-ui-navbar.js',
            '<%= pkg.rooms %>/js/rooms-ui-create.js',
            '<%= pkg.rooms %>/js/rooms-base.js'
          ],
          '<%= pkg.rooms %>/js/room.js': [
            '<%= pkg.rooms %>/js/room-base.js',
            '<%= pkg.rooms %>/js/ui-room-title.js',
            '<%= pkg.rooms %>/js/ui-room-options.js'
          ],
          '<%= pkg.share %>/js/share.js': [
            '<%= pkg.share %>/js/ui-image-upload.js',
            '<%= pkg.share %>/js/share-base.js'
          ],
          'res/js/common.js': [
            'web/app/3rdparty/bootstrap/js/dropdown.js',
            'web/app/3rdparty/bootstrap/js/modal.js',
            'web/app/3rdparty/bootstrap/js/tooltip.js',
            'web/app/client/js/3rdparty/jquery.timeago.js',
            'web/app/client/js/base.js',
            'web/app/client/js/utils.js',
            'web/app/client/js/phpjs.js',
            'res/js/lang.js',
            'web/app/client/js/ui/ui-base.js',
            'web/app/client/js/ui/ui-modal.js',
            'res/js/ChatView.js'
          ],
          'res/js/user.js': ['res/js/ui-connection-dialog.js', 'res/js/user-base.js'],
          'res/js/server.js': ['res/js/ui-oauth-dialog.js', 'res/js/server-base.js']
        }
      }
    },
    uglify: {
      base: {
        files: {
          'res/html/js/PluginsView.min.js': ['res/html/src/PluginsView.js'],
          'res/html/js/about.min.js': ['res/html/src/about.js'],
          'res/js/common.min.js': ['res/js/common.js'],
          'res/js/user.min.js': ['res/js/user.js'],
          'res/js/server.min.js': ['res/js/server.js']
        }
      },
      plugins: {
        files: {
          'src/common/plugins/RawFeeds/res/js/RawFeeds.min.js': ['src/common/plugins/RawFeeds/res/src/KelpJSONView.js', 'src/common/plugins/RawFeeds/res/src/RawFeeds.js'],
          'src/common/plugins/Profile/res/js/Profile.min.js': ['src/common/plugins/Profile/res/src/Profile.js'],
          'src/common/plugins/YouTube/res/js/YouTube.min.js': ['src/common/plugins/YouTube/res/src/YouTube.js'],
          'src/common/plugins/SendFile/res/js/SendFile.min.js': ['src/common/plugins/SendFile/res/src/SendFile.js'],
          'src/common/plugins/History/res/js/History.min.js': ['src/common/plugins/History/res/src/History.js'],
          'src/common/plugins/History/res/js/days.min.js': ['src/common/plugins/History/res/src/days.js'],
          '<%= pkg.rooms %>/js/rooms.min.js': ['<%= pkg.rooms %>/js/rooms.js'],
          '<%= pkg.rooms %>/js/room.min.js': ['<%= pkg.rooms %>/js/room.js'],
          '<%= pkg.rooms %>/js/user.min.js': ['<%= pkg.rooms %>/js/user-base.js'],
          '<%= pkg.share %>/js/share.min.js': ['<%= pkg.share %>/js/share.js']
        }
      }
    },
    cssmin: {
      base: {
        files: {
          'res/html/css/about.min.css': ['res/html/css/about.min.css'],
          'res/html/css/PluginsView.min.css': ['res/html/css/PluginsView.min.css']
        }
      },
      plugins: {
        files: {
          'src/common/plugins/RawFeeds/res/css/RawFeeds.min.css': ['src/common/plugins/RawFeeds/res/src/RawFeeds.css'],
          'src/common/plugins/Profile/res/css/flags.min.css': ['src/common/plugins/Profile/res/src/flags.css'],
          'src/common/plugins/YouTube/res/css/YouTube.min.css': ['src/common/plugins/YouTube/res/src/YouTube.css'],
          'src/common/plugins/SendFile/res/css/SendFile.min.css': ['src/common/plugins/SendFile/res/src/SendFile.css'],
          'src/common/plugins/History/res/css/History.min.css': ['src/common/plugins/History/res/src/History.css']
        }
      }
    },
    recess: {
      options: {
        compile: true,
        rooms: [
          'web/app/3rdparty/bootstrap/less/schat-channels.less',
          'web/app/3rdparty/bootstrap/less/theme.less',
          'res/css/bootstrap-reset.css',
          'res/css/scrollbar.css',
          'web/app/client/css/base.css',
          'web/app/client/css/icons.css',
          'res/css/icons.css',
          'web/app/client/css/index.css',
          '<%= pkg.rooms %>/css/rooms-base.css'
        ],
        common: [
          'web/app/3rdparty/bootstrap/less/schat-common.less',
          'web/app/3rdparty/bootstrap/less/theme.less',
          'res/css/bootstrap-reset.css',
          'res/css/scrollbar.css',
          'web/app/client/css/base.css',
          'web/app/client/css/icons.css',
          'res/css/icons.css',
          'res/css/ChatView.css'
        ]
      },
      compile: {
        files: {
          '<%= pkg.rooms %>/css/rooms.css': ['<%= recess.options.rooms %>'],
          '<%= pkg.rooms %>/css/room.css': ['<%= pkg.rooms %>/css/room-base.css'],
          '<%= pkg.share %>/css/share.css': ['<%= pkg.share %>/css/share-base.css'],
          'res/css/common.css': ['<%= recess.options.common %>'],
          'res/css/channel.css': ['res/css/channel-base.css'],
          'res/css/server.css': ['res/css/server-base.css'],
          'res/css/user.css': ['res/css/user-base.css']
        }
      },
      compress: {
        options: {
          compress: true
        },
        files: {
          '<%= pkg.rooms %>/css/rooms.min.css': ['<%= recess.options.rooms %>'],
          '<%= pkg.rooms %>/css/room.min.css': ['<%= pkg.rooms %>/css/room-base.css'],
          '<%= pkg.share %>/css/share.min.css': ['<%= pkg.share %>/css/share-base.css'],
          'res/css/common.min.css': ['<%= recess.options.common %>'],
          'res/css/channel.min.css': ['res/css/channel-base.css'],
          'res/css/server.min.css': ['res/css/server-base.css'],
          'res/css/user.min.css': ['res/css/user-base.css']
        }
      }
    }
  });

  grunt.loadNpmTasks('grunt-contrib-concat');
  grunt.loadNpmTasks('grunt-contrib-uglify');
  grunt.loadNpmTasks('grunt-contrib-cssmin');
  grunt.loadNpmTasks('grunt-recess');

  grunt.registerTask('default', ['concat', 'uglify', 'cssmin', 'recess']);
};
