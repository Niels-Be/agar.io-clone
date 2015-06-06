module.exports = function (grunt) {
  require('matchdep').filterDev('grunt-*').forEach(grunt.loadNpmTasks);

  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
    coffee: {
      client: {
        options: { 
          bare: false,
          join: true
        },
        files: {
          'client/js/app.js': [
            'client/lib/helper.coffee',
            'client/lib/packets.coffee',
            'client/lib/network.coffee',
            'client/lib/ball.coffee',
            'client/lib/grid.coffee',
            'client/lib/client.coffee'
          ],
        }
      }
    },
    uglify: {
      build: {
        options: {
          preserveComments: 'none'
        },
        files: {
          'client/js/app.min.js': 'client/js/app.js'
        }
      }
    },
    watch: {
      all: {
        files: ['*/lib/**/*.coffee'],
        tasks: 'default'
      },
      dev: {
        files:  '*/lib/*.coffee' ,
        tasks: ['concat:build/server.coffee', 'coffee:lib']
      }
    }
  });

  grunt.registerTask('default', ['coffee', 'uglify']);
};
