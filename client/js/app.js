(function() {
  var Ball, Game, Grid, extend;

  extend = function(object, properties) {
    var key, val;
    for (key in properties) {
      val = properties[key];
      object[key] = val;
    }
    return object;
  };

  Ball = (function() {
    function Ball(options1, data) {
      this.options = options1;
      extend(this, data);
      this.updates = 0;
    }

    Ball.prototype.updateData = function(data) {
      return extend(this, data);
    };

    Ball.prototype.update = function(t) {
      this.updates++;
      if (this.velocity) {
        this.x += this.velocity.x * t;
        return this.y += this.velocity.y * t;
      }
    };

    Ball.prototype.render = function(graph) {
      var fontSize;
      graph.beginPath();
      graph.strokeStyle = this.options.borderColor;
      graph.fillStyle = this.color || this.options.fillColor;
      graph.lineWidth = this.options.border;
      graph.arc(this.x, this.y, this.size, 0, 2 * Math.PI);
      graph.stroke();
      graph.fill();
      if (this.name && this.name !== "") {
        fontSize = Math.max(this.size / 2, this.options.defaultSize);
        graph.lineWidth = this.options.textBorderSize;
        graph.textAlign = "center";
        graph.fillStyle = this.options.textColor;
        graph.textBaseline = 'middle';
        graph.strokeStyle = this.options.textBorder;
        graph.font = "bold " + fontSize + "px sans-serif";
        graph.strokeText(this.name, this.x, this.y);
        return graph.fillText(this.name, this.x, this.y);
      }
    };

    return Ball;

  })();

  Grid = (function() {
    function Grid(game, options1) {
      this.game = game;
      this.options = options1;
    }

    Grid.prototype.render = function(graph) {
      var j, k, ref, ref1, ref2, ref3, x, y;
      graph.beginPath();
      graph.lineWidth = 1;
      graph.strokeStyle = this.options.lineColor;
      for (x = j = 0, ref = this.game.gamefield.width, ref1 = this.options.size; ref1 > 0 ? j <= ref : j >= ref; x = j += ref1) {
        graph.moveTo(x, 0);
        graph.lineTo(x, this.game.gamefield.height);
      }
      for (y = k = 0, ref2 = this.game.gamefield.height, ref3 = this.options.size; ref3 > 0 ? k <= ref2 : k >= ref2; y = k += ref3) {
        graph.moveTo(0, y);
        graph.lineTo(this.game.gamefield.width, y);
      }
      graph.stroke();
      graph.beginPath();
      graph.lineWidth = this.options.borderSize;
      graph.strokeStyle = this.options.borderColor;
      graph.moveTo(0, 0);
      graph.lineTo(0, this.game.gamefield.height);
      graph.lineTo(this.game.gamefield.width, this.game.gamefield.height);
      graph.lineTo(this.game.gamefield.width, 0);
      graph.lineTo(0, 0);
      return graph.stroke();
    };


    /*	px = @game.player.x % @options.size
    		py = @game.player.y % @options.size
    
    		graph.beginPath()
    		graph.lineWidth = 1
    		graph.strokeStyle = @options.lineColor
    		for(x = 0 x <= view.width x += @options.size) {
    			graph.moveTo(0 + x - px, 0)
    			graph.lineTo(0 + x - px, view.height)
    		}
    		for(y = 0 y <= view.height y += @options.size) {
    			graph.moveTo(0, 0 + y - py)
    			graph.lineTo(view.width, 0 + y - py)
    		}
    		graph.stroke()
    
    		graph.beginPath()
    		graph.lineWidth = @options.borderSize
    		graph.strokeStyle = @options.borderColor
    		graph.moveTo(view.width/2 - @game.player.x, 0)
    		graph.lineTo(view.width/2 - @game.player.x, view.height)
    
    		graph.moveTo(view.width/2 - @game.player.x + gamefieldSize, 0)
    		graph.lineTo(view.width/2 - @game.player.x + gamefieldSize, view.height)
    
    		graph.moveTo(0, view.height/2 - @game.player.y + gamefieldSize)
    		graph.lineTo(view.width, view.height/2 - @game.player.y + gamefieldSize)
    
    		graph.moveTo(0, view.height/2 - @game.player.y)
    		graph.lineTo(view.width, view.height/2 - @game.player.y)
    
    		graph.stroke()
     */

    return Grid;

  })();

  Game = (function() {
    Game.prototype.lobbySocket = io();

    Game.prototype.socket = null;

    Game.prototype.screen = {
      width: window.innerWidth,
      height: window.innerHeight
    };

    Game.prototype.view = {
      width: window.innerWidth,
      height: window.innerHeight
    };

    Game.prototype.gameStarted = false;

    Game.prototype.inRoom = false;

    Game.defaultOptions = {
      viewPortScaleFactor: 0.01,
      backgroundColor: '#EEEEEE',
      grid: {
        lineColor: '#DADADA',
        borderColor: '#A0A0A0',
        borderSize: 5,
        size: 50
      },
      food: {
        border: 2,
        borderColor: "#f39c12",
        fillColor: "#f1c40f",
        size: 5
      },
      player: {
        border: 3,
        borderColor: "#c0392b",
        fillColor: "#ea6153",
        textColor: "#FFFFFF",
        textBorder: "#000000",
        textBorderSize: 3,
        defaultSize: 10
      },
      enemy: {
        border: 3,
        borderColor: "#27ae60",
        fillColor: "#c0392b",
        textColor: "#FFFFFF",
        textBorder: "#000000",
        textBorderSize: 3,
        defaultSize: 10
      }
    };

    Game.prototype.player = {
      x: 0,
      y: 0,
      size: 0
    };

    Game.prototype.elements = {
      "static": {},
      moveable: {}
    };

    Game.prototype.target = {
      x: 0,
      y: 0
    };

    Game.prototype.fps = 0;

    Game.prototype.fpstimer = 0;

    Game.prototype.lastTick = 0;

    Game.prototype.name = "NoName";

    Game.prototype.rooms = {};

    Game.prototype.lastRoom = 0;

    function Game(options) {
      this.options = extend(extend({}, Game.defaultOptions), options);
      this.grid = new Grid(this, this.options.grid);
      this.init();
      this.lobbySocket.emit("getRooms");
      this.lobbySocket.on("availableRooms", (function(_this) {
        return function(rooms) {
          var i, ref, room;
          _this.rooms = extend(rooms, _this.rooms);
          console.log("Rooms:", _this.rooms);
          _this.roomsText.innerHTML = "";
          ref = _this.rooms;
          for (i in ref) {
            room = ref[i];
            _this.roomsText.innerHTML += "<option value=\"" + i + "\" " + (_this.lastRoom === i ? "selected=\"selected\"" : "") + ">" + room.name + " (" + room.playercount + ")</option>";
          }
          return _this.join(_this.lastRoom);
        };
      })(this));
      this.loop();
    }

    Game.prototype.setCallbacks = function(socket) {
      this.socket = socket;
      this.socket.on("connect", (function(_this) {
        return function() {
          return console.log("Connected");
        };
      })(this));
      this.socket.on("reconnect", (function(_this) {
        return function() {
          return console.log("Reconnected");
        };
      })(this));
      this.socket.on("playerJoined", (function(_this) {
        return function(ply) {
          return console.log("A player joind:", ply);
        };
      })(this));
      this.socket.on("start", (function(_this) {
        return function(player) {
          _this.player = player;
          _this.updatePlayer();
          return _this.gameStarted = true;
        };
      })(this));
      this.socket.on("createFood", (function(_this) {
        return function(food) {
          return _this.elements["static"][food.id] = new Ball(_this.options.food, food);
        };
      })(this));
      this.socket.on("deleteElement", (function(_this) {
        return function(id) {
          if (_this.elements["static"].hasOwnProperty(id)) {
            delete _this.elements["static"][id];
          }
          if (_this.elements.moveable.hasOwnProperty(id)) {
            return delete _this.elements.moveable[id];
          }
        };
      })(this));
      this.socket.on("updateStatics", (function(_this) {
        return function(objs) {
          var j, len, o, results;
          _this.elements["static"] = {};
          results = [];
          for (j = 0, len = objs.length; j < len; j++) {
            o = objs[j];
            results.push(_this.elements["static"][o.id] = new Ball(_this.options.food, o));
          }
          return results;
        };
      })(this));
      this.socket.on("updateMoveables", (function(_this) {
        return function(objs) {
          var b, j, k, len, len1, o, ref, results;
          _this.elements.moveable = {};
          for (j = 0, len = objs.length; j < len; j++) {
            o = objs[j];
            _this.elements.moveable[o.id] = new Ball(_this.options.enemy, o);
          }
          if (_this.player.balls) {
            ref = _this.player.balls;
            results = [];
            for (k = 0, len1 = ref.length; k < len1; k++) {
              b = ref[k];
              if (_this.elements.moveable.hasOwnProperty(b)) {
                results.push(_this.elements.moveable[b].options = _this.options.player);
              }
            }
            return results;
          }
        };
      })(this));
      this.socket.on("updatePlayer", (function(_this) {
        return function(player) {
          _this.player = player;
          _this.massText.innerHTML = "Mass: " + _this.player.mass;
          return _this.updatePlayer();
        };
      })(this));
      this.socket.on("rip", (function(_this) {
        return function() {
          _this.gameStarted = false;
          spawnbox.hidden = false;
          return _this.lobbySocket.emit("getRooms");
        };
      })(this));
      this.socket.on("disconnect", (function(_this) {
        return function() {
          console.log("Disconnected");
          _this.socket.close();
          return _this.inRoom = false;
        };
      })(this));
      this.socket.on("error", (function(_this) {
        return function(err) {
          return console.log("Error:", err);
        };
      })(this));
      return this.socket.on("stats", (function(_this) {
        return function(stats) {
          return console.log("Stats", stats);
        };
      })(this));
    };

    Game.prototype.init = function() {
      this.canvas = document.getElementById("cvs");
      this.canvas.addEventListener("mousemove", (function(_this) {
        return function(evt) {
          _this.target.x = evt.clientX - _this.screen.width / 2;
          return _this.target.y = evt.clientY - _this.screen.height / 2;
        };
      })(this));
      this.canvas.addEventListener("keypress", (function(_this) {
        return function(evt) {
          if (evt.charCode === 32) {
            _this.socket.emit("splitUp", _this.target);
          }
          if (evt.charCode === 119) {
            return _this.socket.emit("shoot", _this.target);
          }
        };
      })(this));
      this.canvas.width = this.screen.width;
      this.canvas.height = this.screen.height;
      window.addEventListener("resize", (function(_this) {
        return function() {
          _this.screen.width = window.innerWidth;
          _this.screen.height = window.innerHeight;
          _this.canvas.width = window.innerWidth;
          return _this.canvas.height = window.innerHeight;
        };
      })(this));
      this.graph = this.canvas.getContext("2d");
      this.massText = document.getElementById("mass");
      this.statusText = document.getElementById("status");
      this.roomsText = document.getElementById("rooms");
      this.nameText = document.getElementById("name");
      return this.spawnbox = document.getElementById("spawnbox");
    };

    Game.prototype.join = function(index) {
      console.log("Joining Room " + this.rooms[index].name);
      this.gamefield = this.rooms[index].options;
      if (this.inRoom && index !== this.lastRoom) {
        this.socket.emit("leave");
        this.socket.disconnect();
        this.inRoom = false;
        setTimeout(((function(_this) {
          return function() {
            return _this.join(index);
          };
        })(this)), 10);
        return;
      }
      console.log("Connecting ...");
      if (this.rooms[index].socket) {
        if (!this.rooms[index].socket.connected) {
          this.rooms[index].socket.connect();
        }
        this.socket = this.rooms[index].socket;
      } else {
        this.rooms[index].socket = io.connect('/' + this.rooms[index].name);
        this.setCallbacks(this.rooms[index].socket);
      }
      this.socket.emit("join");
      this.inRoom = true;
      this.lastRoom = index;
      return this.updatePlayer();
    };

    Game.prototype.start = function() {
      console.log("Starting Game");
      this.name = this.nameText.value;
      this.socket.emit("start", this.name);
      spawnbox.hidden = true;
      return this.canvas.focus();
    };

    Game.prototype.loop = function() {
      return window.requestAnimationFrame((function(_this) {
        return function(now) {
          var timediff;
          timediff = now - _this.lastTick;
          if (_this.inRoom) {
            _this.update(timediff * 1e-3);
            _this.render();
          }
          _this.lastTick = now;
          return _this.loop();
        };
      })(this));
    };

    Game.prototype.update = function(timediff) {
      var i, m, ref;
      ref = this.elements.moveable;
      for (i in ref) {
        m = ref[i];
        m.update(timediff);
      }
      this.updatePlayer();
      this.socket.emit("updateTarget", this.target);
      this.fpstimer += timediff;
      this.fps++;
      if (this.fpstimer > 1) {
        this.statusText.innerHTML = "FPS:" + this.fps;
        this.fps = 0;
        return this.fpstimer = 0;
      }
    };

    Game.prototype.updatePlayer = function() {
      var b, j, len, ref, results;
      if (this.gameStarted) {
        this.player.x = this.player.y = this.player.size = 0;
        ref = this.player.balls;
        results = [];
        for (j = 0, len = ref.length; j < len; j++) {
          b = ref[j];
          if (!(this.elements.moveable.hasOwnProperty(b))) {
            continue;
          }
          this.player.x += this.elements.moveable[b].x / this.player.balls.length;
          this.player.y += this.elements.moveable[b].y / this.player.balls.length;
          results.push(this.player.size += this.elements.moveable[b].size / this.player.balls.length);
        }
        return results;
      } else {
        this.player.x = this.gamefield.width / 2;
        this.player.y = this.gamefield.height / 2;
        return this.player.size = Math.log(Math.min(this.gamefield.width - this.screen.width, this.gamefield.height - this.screen.height)) / this.options.viewPortScaleFactor / 2;
      }
    };

    Game.prototype.render = function() {
      var i, m, ref, ref1, results, s, scale;
      this.graph.setTransform(1, 0, 0, 1, 0, 0);
      this.graph.fillStyle = this.options.backgroundColor;
      this.graph.fillRect(0, 0, this.screen.width, this.screen.height);
      scale = 1 / (this.options.viewPortScaleFactor * this.player.size + 1);
      this.graph.setTransform(scale, 0, 0, scale, this.screen.width / 2 - this.player.x * scale, this.screen.height / 2 - this.player.y * scale);
      this.grid.render(this.graph);
      ref = this.elements["static"];
      for (i in ref) {
        s = ref[i];
        s.render(this.graph);
      }
      ref1 = this.elements.moveable;
      results = [];
      for (i in ref1) {
        m = ref1[i];
        results.push(m.render(this.graph));
      }
      return results;
    };

    Game.prototype.getStats = function() {
      return this.socket.emit("getStats");
    };

    return Game;

  })();

  (function() {
    var lastTime = 0;
    var vendors = ['webkit', 'moz'];
    for(var x = 0; x < vendors.length && !window.requestAnimationFrame; ++x) {
        window.requestAnimationFrame = window[vendors[x]+'RequestAnimationFrame'];
        window.cancelAnimationFrame =
          window[vendors[x]+'CancelAnimationFrame'] || window[vendors[x]+'CancelRequestAnimationFrame'];
    }

    if (!window.requestAnimationFrame)
        window.requestAnimationFrame = function(callback, element) {
            var currTime = new Date().getTime();
            var timeToCall = Math.max(0, 16 - (currTime - lastTime));
            var id = window.setTimeout(function() { callback(currTime + timeToCall); },
              timeToCall);
            lastTime = currTime + timeToCall;
            return id;
        };

    if (!window.cancelAnimationFrame)
        window.cancelAnimationFrame = function(id) {
            clearTimeout(id);
        };
}());;

  this.game = new Game();

}).call(this);
