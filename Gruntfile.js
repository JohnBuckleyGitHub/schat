module.exports = function(grunt) {
  'use strict';

  grunt.initConfig({
    concat: {
      roomsJs: {
        src: [
          'web/app/client/js/base.js',
          'web/app/client/js/utils.js',
          'web/app/client/js/phpjs.js',
          'web/app/client/js/lang.js',
          'web/app/client/js/ui/ui-base.js',
          'web/app/client/js/ui/ui-index.js',
          'src/common/plugins/Channels/res/src/rooms-ui-navbar.js',
          'src/common/plugins/Channels/res/src/rooms-base.js'
        ],
        dest: 'src/common/plugins/Channels/res/src/rooms.js'
      },
      roomsCss: {
        src: [
          'web/app/client/css/base.css',
          'web/app/client/css/index.css',
          'src/common/plugins/Channels/res/src/rooms-base.css'
        ],
        dest: 'src/common/plugins/Channels/res/src/rooms.css'
      }
    },
    uglify: {
      base: {
        files: {
          'res/html/js/ChatView.min.js': ['res/html/src/ChatView.js'],
          'res/html/js/PluginsView.min.js': ['res/html/src/PluginsView.js'],
          'res/html/js/server.min.js': ['res/html/src/server.js'],
          'res/html/js/user.min.js': ['res/html/src/user.js'],
          'res/html/js/about.min.js': ['res/html/src/about.js'],
          'res/html/js/bootstrap-tooltip.min.js': ['res/html/src/bootstrap-tooltip.js'],
          'res/html/js/bootstrap-modal.min.js': ['res/html/src/bootstrap-modal.js'],
          'res/html/js/jquery.timeago.min.js': ['res/html/src/jquery.timeago.js'],
          'res/html/js/jquery.timeago.en.min.js': ['res/html/src/jquery.timeago.en.js'],
          'res/html/js/jquery.timeago.ru.min.js': ['res/html/src/jquery.timeago.ru.js']
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
          'src/common/plugins/Channels/res/js/rooms.min.js': ['<%= concat.roomsJs.dest %>']
        }
      }
    },
    cssmin: {
      base: {
        files: {
          'res/html/css/about.min.css': ['res/html/css/about.min.css'],
          'res/html/css/PluginsView.min.css': ['res/html/css/PluginsView.min.css'],
          'res/html/css/ChatView.min.css': ['res/html/css/ChatView.min.css'],
          'res/html/css/server.min.css': ['res/html/css/server.min.css'],
          'res/html/css/user.min.css': ['res/html/css/user.min.css'],
          'res/html/css/channel.min.css': ['res/html/css/channel.min.css'],
          'src/common/plugins/Channels/res/css/rooms.min.css': ['<%= concat.roomsCss.dest %>']
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
        compile: true
      },
      dev: {
        files: {
          'src/common/plugins/Channels/res/src/bootstrap.css': [
            'web/app/3rdparty/bootstrap/less/schat-channels.less',
            'web/app/3rdparty/bootstrap/less/theme.less',
            'res/html/src/bootstrap-reset.css'
          ]
        }
      },
      min: {
        options: {
          compress: true
        },
        files: {
          'src/common/plugins/Channels/res/css/bootstrap.min.css': [
            'web/app/3rdparty/bootstrap/less/schat-channels.less',
            'web/app/3rdparty/bootstrap/less/theme.less',
            'res/html/src/bootstrap-reset.css'
          ]
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
