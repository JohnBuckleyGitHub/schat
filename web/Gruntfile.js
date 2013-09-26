module.exports = function(grunt) {
  'use strict';

  grunt.initConfig({

    concat: {
      js: {
        src: [
          'app/id/base32.js',
          'node_modules/node-uuid/uuid.js',
          'app/client/js/base.js',
          'app/client/js/router.js',
          'app/client/js/utils.js',
          'app/net/memory-cache.js',
          'app/client/js/phpjs.js',
          'app/client/js/lang-en.js',
          'app/client/js/lang-ru.js',
          'app/client/js/lang.js',
          'app/net/packets.js',
          'app/client/js/client.js',
          'app/client/js/index.js',
          'app/client/js/auth.js',
          'app/client/js/stats.js',
          'app/client/js/users.js',
          'app/client/js/user.js',
          'app/client/js/ui/ui-base.js',
          'app/client/js/ui/ui-loader.js',
          'app/client/js/ui/ui-index.js',
          'app/client/js/ui/ui-oauth.js',
          'app/client/js/ui/ui-welcome.js',
          'app/client/js/ui/ui-stats.js',
          'app/client/js/ui/ui-user-list.js',
          'app/client/js/ui/ui-users.js',
          'app/client/js/ui/ui-user.js',
          'app/client/js/ready.js',
        ],
        dest: 'public/js/schat.js'
      },
      css: {
        src: [
          'app/client/css/navbar.css',
          'app/client/css/base.css',
          'app/client/css/icons.css',
          'app/client/css/loader.css',
          'app/client/css/oauth.css',
          'app/client/css/signin.css',
          'app/client/css/index.css',
          'app/client/css/stats.css',
          'app/client/css/users.css',
          'app/client/css/user.css',
          'app/client/css/schat@2x.css'
        ],
        dest: 'public/css/schat.css'
      },
      jquery: {
        src: [
          'app/client/js/3rdparty/jquery.timeago.js'
        ],
        dest: 'public/js/jquery.plugins.js'
      }
    },
    recess: {
      options: {
        compile: true
      },
      dev: {
        files: {
          'public/css/bootstrap.css': ['app/3rdparty/bootstrap/less/schat-web.less', 'app/3rdparty/bootstrap/less/theme.less']
        }
      },
      min: {
        options: {
          compress: true
        },
        files: {
          'public/css/bootstrap.min.css': ['app/3rdparty/bootstrap/less/schat-web.less', 'app/3rdparty/bootstrap/less/theme.less']
        }
      }
    },
    jshint: {
      js: {
        src: ['<%= concat.js.src %>'],
        filter: function(filepath) {
          return filepath.indexOf('uuid.js') == -1;
        }
      },
      jquery: {
        src: ['<%= concat.jquery.src %>']
      }
    },
    uglify: {
      js: {
        files: {
          'public/js/schat.min.js': ['<%= concat.js.dest %>']
        }
      },
      jquery: {
        files: {
          'public/js/jquery.plugins.min.js': ['<%= concat.jquery.dest %>']
        }
      }
    },
    cssmin: {
      combine: {
        options: {
          report: 'min'
        },
        files: {
          'public/css/schat.min.css': ['<%= concat.css.dest %>']
        }
      }
    },
    watch: {
      js: {
        files: ['<%= concat.js.src %>'],
        tasks: ['concat:js', 'jshint', 'uglify']
      },
      css: {
        files: ['<%= concat.css.src %>'],
        tasks: ['concat:css', 'cssmin']
      }
    }
  });

  grunt.loadNpmTasks('grunt-contrib-concat');
  grunt.loadNpmTasks('grunt-contrib-jshint');
  grunt.loadNpmTasks('grunt-contrib-uglify');
  grunt.loadNpmTasks('grunt-contrib-cssmin');
  grunt.loadNpmTasks('grunt-contrib-watch');
  grunt.loadNpmTasks('grunt-recess');

  grunt.registerTask('default', ['concat', 'recess', 'jshint', 'uglify', 'cssmin']);
};
