module.exports = function (grunt) {
  require('matchdep').filterDev('grunt-*').forEach(grunt.loadNpmTasks);

  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
    coffee: {
      server: {
        options: { 
          bare: true,
          join: true
        },
        files: {
          'server/server.js': [
            'server/lib/helper.coffee',
            'server/lib/player.coffee',
            'server/lib/element.coffee',
            'server/lib/gamefield.coffee',
            'server/lib/food.coffee',
            'server/lib/ball.coffee',
            'server/lib/server.coffee'
          ],
        }
      },
      client: {
        options: { 
          bare: false,
          join: true
        },
        files: {
          'client/js/app.js': [
            'client/lib/helper.coffee',
            'client/lib/ball.coffee',
            'client/lib/grid.coffee',
            'client/lib/client.coffee'
          ],
        }
      },
      spec: {
        options: { 
          bare: true, 
          join: true 
        },
        files: {
          'build/spec.js': ['spec/support/**/*.coffee', 'spec/lib/**/*.coffee']
        }
      },
    },
    uglify: {
      build: {
        options: {
          preserveComments: 'none'
        },
        files: {
          'server/server.min.js': 'server/server.js',
          'client/js/app.min.js': 'client/js/app.js'
        }
      }
    },
    mocha: {
      index: ['spec/specs.html'],
      options: {run: true}
    },
    watch: {
      all: {
        files: ['*/lib/**/*.coffee', 'spec/lib/**/*.coffee', 'spec/support/**/*.coffee', 'less/**/*.less'],
        tasks: 'default'
      },
      dev: {
        files:  '*/lib/*.coffee' ,
        tasks: ['concat:build/server.coffee', 'coffee:lib']
      }
    }
  });

  grunt.registerTask('default', ['coffee', 'uglify', 'mocha']);
};
