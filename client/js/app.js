(function() {
  var Ball, Game, Grid, JoinPacket, JsonPacket, Network, Packet, PlayerUpdatePacket, SetElementsPacket, StartPacket, StatsPacket, TargetPacket, UpdateElementsPacket, extend, stringToUint, uintToString,
    extend1 = function(child, parent) { for (var key in parent) { if (hasProp.call(parent, key)) child[key] = parent[key]; } function ctor() { this.constructor = child; } ctor.prototype = parent.prototype; child.prototype = new ctor(); child.__super__ = parent.prototype; return child; },
    hasProp = {}.hasOwnProperty;

  extend = function(object, properties) {
    var key, val;
    for (key in properties) {
      val = properties[key];
      object[key] = val;
    }
    return object;
  };

  stringToUint = function(string) {
    var c, charList, k, len, uintArray;
    string = unescape(encodeURIComponent(string));
    charList = string.split('');
    uintArray = [];
    for (k = 0, len = charList.length; k < len; k++) {
      c = charList[k];
      uintArray.push(c.charCodeAt(0));
    }
    return uintArray;
  };

  uintToString = function(uintArray) {
    var encodedString;
    encodedString = String.fromCharCode.apply(null, uintArray);
    return decodeURIComponent(escape(encodedString));
  };

  Packet = (function() {
    function Packet(id1) {
      this.id = id1;
    }

    Packet.prototype.parseData = function(data) {};

    Packet.prototype.getData = function() {
      var ar, dv;
      ar = new ArrayBuffer(1);
      dv = new DataView(ar);
      dv.setUint8(0, this.id);
      return ar;
    };

    return Packet;

  })();

  JoinPacket = (function(superClass) {
    extend1(JoinPacket, superClass);

    function JoinPacket(lobby) {
      this.lobby = lobby;
      JoinPacket.__super__.constructor.call(this, 0x10);
    }

    JoinPacket.prototype.getData = function() {
      var ar, dv;
      ar = new ArrayBuffer(1 + 4);
      dv = new DataView(ar);
      dv.setUint8(0, this.id);
      dv.setUint32(1, this.lobby, true);
      return ar;
    };

    return JoinPacket;

  })(Packet);

  StartPacket = (function(superClass) {
    extend1(StartPacket, superClass);

    function StartPacket(name1) {
      this.name = name1;
      StartPacket.__super__.constructor.call(this, 0x12);
    }

    StartPacket.prototype.getData = function() {
      var ar, strbuf;
      strbuf = stringToUint(this.name);
      ar = new Uint8Array(1 + strbuf.length);
      ar.set([this.id], 0);
      ar.set(strbuf, 1);
      return ar.buffer;
    };

    return StartPacket;

  })(Packet);

  TargetPacket = (function(superClass) {
    extend1(TargetPacket, superClass);

    function TargetPacket(x1, y1) {
      this.x = x1;
      this.y = y1;
      TargetPacket.__super__.constructor.call(this, 0x20);
    }

    TargetPacket.prototype.getData = function() {
      var ar, dv;
      ar = new ArrayBuffer(1 + 8 + 8);
      dv = new DataView(ar);
      dv.setUint8(0, this.id);
      dv.setFloat64(1, this.x, true);
      dv.setFloat64(9, this.y, true);
      return ar;
    };

    return TargetPacket;

  })(Packet);

  PlayerUpdatePacket = (function(superClass) {
    extend1(PlayerUpdatePacket, superClass);

    function PlayerUpdatePacket() {
      PlayerUpdatePacket.__super__.constructor.call(this, 0x24);
    }

    PlayerUpdatePacket.prototype.parseData = function(data) {
      var pos, results;
      this.mass = data.getUint32(1, true);
      this.balls = [];
      pos = 5;
      results = [];
      while (pos < data.byteLength) {
        this.balls.push(data.getUint32(pos, true));
        results.push(pos += 4);
      }
      return results;
    };

    return PlayerUpdatePacket;

  })(Packet);

  SetElementsPacket = (function(superClass) {
    extend1(SetElementsPacket, superClass);

    function SetElementsPacket() {
      SetElementsPacket.__super__.constructor.call(this, 0x30);
    }

    SetElementsPacket.prototype.parseData = function(data) {
      var e, pos, ref, results;
      this.elements = [];
      pos = 1;
      results = [];
      while (pos < data.byteLength) {
        ref = Network.parseElementData(data, pos), pos = ref[0], e = ref[1];
        results.push(this.elements.push(e));
      }
      return results;
    };

    return SetElementsPacket;

  })(Packet);

  UpdateElementsPacket = (function(superClass) {
    extend1(UpdateElementsPacket, superClass);

    function UpdateElementsPacket() {
      UpdateElementsPacket.__super__.constructor.call(this, 0x31);
    }

    UpdateElementsPacket.prototype.parseData = function(data) {
      var count, e, k, l, pos, ref, ref1, ref2, ref3, results;
      this.newElements = [];
      this.deletedElements = [];
      this.updateElements = [];
      count = data.getUint16(1, true);
      pos = 3;
      for (k = 0, ref = count; 0 <= ref ? k < ref : k > ref; 0 <= ref ? k++ : k--) {
        ref1 = Network.parseElementData(data, pos), pos = ref1[0], e = ref1[1];
        this.newElements.push(e);
      }
      count = data.getUint16(pos, true);
      pos += 2;
      for (l = 0, ref2 = count; 0 <= ref2 ? l < ref2 : l > ref2; 0 <= ref2 ? l++ : l--) {
        this.deletedElements.push(data.getUint32(pos, true));
        pos += 4;
      }
      results = [];
      while (pos < data.byteLength) {
        ref3 = Network.parseElementUpdateData(data, pos), pos = ref3[0], e = ref3[1];
        results.push(this.updateElements.push(e));
      }
      return results;
    };

    return UpdateElementsPacket;

  })(Packet);

  StatsPacket = (function(superClass) {
    extend1(StatsPacket, superClass);

    function StatsPacket() {
      StatsPacket.__super__.constructor.call(this, 0xF0);
    }

    StatsPacket.prototype.parseData = function(data) {
      this.update = data.getFloat64(1, true);
      this.collision = data.getFloat64(1 + 8, true);
      this.other = data.getFloat64(1 + 8 + 8, true);
      this.elementCount = data.getUint32(1 + 8 + 8 + 8, true);
      return this.playerCount = data.getUint32(1 + 8 + 8 + 8 + 4, true);
    };

    return StatsPacket;

  })(Packet);

  JsonPacket = (function(superClass) {
    extend1(JsonPacket, superClass);

    function JsonPacket(id1, data1) {
      this.id = id1;
      this.data = data1;
      JsonPacket.__super__.constructor.call(this, this.id);
    }

    JsonPacket.prototype.getData = function() {
      var ar, strbuf;
      console.log("JsonData", this.id, this.data);
      if (this.data) {
        strbuf = stringToUint(JSON.stringify(this.data));
        ar = new Uint8Array(1 + strbuf.length);
        ar.set([this.id], 0);
        ar.set(strbuf, 1);
        return ar.buffer;
      } else {
        return JsonPacket.__super__.getData.call(this);
      }
    };

    JsonPacket.prototype.parseData = function(data) {
      var pos, ref, str;
      ref = Network.parseString(data, 1), pos = ref[0], str = ref[1];
      return this.data = JSON.parse(str);
    };

    return JsonPacket;

  })(Packet);

  Network = (function() {
    Network.Packets = {
      Join: 0x10,
      0x10: Packet.bind(void 0, 0x10),
      Leave: 0x11,
      0x11: Packet.bind(void 0, 0x11),
      Start: 0x12,
      0x12: StartPacket.bind(void 0),
      GetLobby: 0x13,
      0x13: JsonPacket.bind(void 0, 0x13),
      UpdateTarget: 0x20,
      0x20: TargetPacket,
      SplitUp: 0x21,
      0x21: Packet.bind(void 0, 0x21),
      Shoot: 0x22,
      0x22: Packet.bind(void 0, 0x22),
      RIP: 0x23,
      0x23: Packet.bind(void 0, 0x23),
      PlayerUpdate: 0x24,
      0x24: PlayerUpdatePacket,
      SetElements: 0x30,
      0x30: SetElementsPacket,
      UpdateElements: 0x31,
      0x31: UpdateElementsPacket,
      GetStats: 0xF0,
      0xF0: StatsPacket
    };

    function Network(uri) {
      this.callbacks = {};
      this.ws = new WebSocket(uri);
      this.ws.binaryType = "arraybuffer";
      this.ws.onopen = (function(_this) {
        return function() {
          if (_this.onConnect) {
            return _this.onConnect();
          }
        };
      })(this);
      this.ws.onclose = (function(_this) {
        return function() {
          if (_this.onDisconnect) {
            return _this.onDisconnect();
          }
        };
      })(this);
      this.ws.onerror = (function(_this) {
        return function(err) {
          return console.err("Error:", err);
        };
      })(this);
      this.ws.onmessage = (function(_this) {
        return function(e) {
          var packet;
          packet = _this.decode(e.data);
          return _this.callbacks[packet.id](packet);
        };
      })(this);
    }

    Network.prototype.onConnect = function(onConnect) {
      this.onConnect = onConnect;
    };

    Network.prototype.onDisconnect = function(onDisconnect) {
      this.onDisconnect = onDisconnect;
    };

    Network.prototype.on = function(id, func) {
      return this.callbacks[id] = func;
    };

    Network.prototype.emit = function(packet) {
      if (typeof packet === "number") {
        packet = new Network.Packets[packet]();
      }
      return this.ws.send(packet.getData());
    };

    Network.prototype.decode = function(data) {
      var dv, err, pack;
      dv = new DataView(data);
      try {
        pack = new Network.Packets[dv.getUint8(0)]();
        pack.parseData(dv);
        return pack;
      } catch (_error) {
        err = _error;
        return console.error("Packet decode error", dv.getUint8(0), err);
      }
    };

    Network.parseString = function(dv, pos) {
      var c, str;
      str = [];
      while (true) {
        c = dv.getUint8(pos);
        pos++;
        if (c === 0) {
          break;
        }
        str.push(c);
      }
      return [pos, uintToString(str)];
    };

    Network.parseElementData = function(dv, pos) {
      var ref, ref1, res;
      res = {};
      res.id = dv.getUint32(pos, true);
      pos += 4;
      res.type = dv.getUint8(pos);
      pos += 1;
      ref = Network.parseString(dv, pos), pos = ref[0], res.color = ref[1];
      ref1 = Network.parseString(dv, pos), pos = ref1[0], res.name = ref1[1];
      res.x = dv.getFloat64(pos, true);
      pos += 8;
      res.y = dv.getFloat64(pos, true);
      pos += 8;
      res.size = dv.getFloat64(pos, true);
      pos += 8;
      return [pos, res];
    };

    Network.parseElementUpdateData = function(dv, pos) {
      var res;
      res = {};
      res.id = dv.getUint32(pos, true);
      pos += 4;
      res.x = dv.getFloat64(pos, true);
      pos += 8;
      res.y = dv.getFloat64(pos, true);
      pos += 8;
      res.size = dv.getFloat64(pos, true);
      pos += 8;
      res.velX = dv.getFloat64(pos, true);
      pos += 8;
      res.velY = dv.getFloat64(pos, true);
      pos += 8;
      return [pos, res];
    };

    return Network;

  })();

  Ball = (function() {
    function Ball(options1, data) {
      this.options = options1;
      extend(this, data);
    }

    Ball.prototype.updateData = function(data) {
      return extend(this, data);
    };

    Ball.prototype.update = function(t) {
      if (this.velX) {
        this.x += this.velX * t;
      }
      if (this.velY) {
        return this.y += this.velY * t;
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
      var k, l, ref, ref1, ref2, ref3, x, y;
      graph.beginPath();
      graph.lineWidth = 1;
      graph.strokeStyle = this.options.lineColor;
      for (x = k = 0, ref = this.game.gamefield.width, ref1 = this.options.size; ref1 > 0 ? k <= ref : k >= ref; x = k += ref1) {
        graph.moveTo(x, 0);
        graph.lineTo(x, this.game.gamefield.height);
      }
      for (y = l = 0, ref2 = this.game.gamefield.height, ref3 = this.options.size; ref3 > 0 ? l <= ref2 : l >= ref2; y = l += ref3) {
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
    Game.prototype.net = new Network("ws://" + window.location.host + "/websocket/");

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

    Game.ElementTypes = {
      0: "ball",
      1: "food",
      2: "shoot",
      3: "obstracle",
      4: "item"
    };

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
      obstracle: {
        border: 5,
        borderColor: "#006600",
        fillColor: "#00FF00"
      },
      shoot: {
        border: 2,
        borderColor: "#f39c12",
        fillColor: "#FF0000"
      },
      player: {
        border: 5,
        borderColor: "#FFFFCC",
        fillColor: "#ea6153",
        textColor: "#FFFFFF",
        textBorder: "#000000",
        textBorderSize: 3,
        defaultSize: 10
      },
      ball: {
        border: 5,
        borderColor: "#CC0000",
        fillColor: "#c0392b",
        textColor: "#FFFFFF",
        textBorder: "#000000",
        textBorderSize: 3,
        defaultSize: 10
      },
      item: {
        border: 5,
        borderColor: "#6666FF",
        fillColor: "#0000FF"
      }
    };

    Game.prototype.player = {
      x: 0,
      y: 0,
      size: 0,
      mass: 0,
      balls: {}
    };

    Game.prototype.elements = {};

    Game.prototype.target = {
      x: 0,
      y: 0,
      changed: false
    };

    Game.prototype.fps = 0;

    Game.prototype.fpstimer = 0;

    Game.prototype.lastTick = 0;

    Game.prototype.name = "NoName";

    Game.prototype.rooms = {};

    function Game(options) {
      this.options = extend(extend({}, Game.defaultOptions), options);
      this.grid = new Grid(this, this.options.grid);
      this.init();
      this.net.onConnect((function(_this) {
        return function() {
          console.log("Connected");
          return _this.net.emit(Network.Packets.GetLobby);
        };
      })(this));
      this.net.on(Network.Packets.GetLobby, (function(_this) {
        return function(packet) {
          var i, k, len, ref, ref1, room;
          _this.rooms = {};
          if (!_this.lastRoom) {
            _this.lastRoom = packet.data[packet.data.length - 1].id;
          }
          ref = packet.data;
          for (k = 0, len = ref.length; k < len; k++) {
            room = ref[k];
            _this.rooms[room.id] = room;
          }
          console.log("Rooms:", _this.rooms);
          _this.roomsText.innerHTML = "";
          ref1 = _this.rooms;
          for (i in ref1) {
            room = ref1[i];
            _this.roomsText.innerHTML += "<option value=\"" + room.id + "\" " + (_this.lastRoom === room.id ? "selected=\"selected\"" : "") + ">" + room.name + " (" + room.playerCount + ")</option>";
          }
          return _this.join(_this.lastRoom);
        };
      })(this));
      this.net.on(Network.Packets.Start, (function(_this) {
        return function() {
          console.log("Game Started");
          return _this.gameStarted = true;
        };
      })(this));
      this.net.on(Network.Packets.SetElements, (function(_this) {
        return function(packet) {
          var k, len, o, ref, results;
          _this.elements = {};
          ref = packet.elements;
          results = [];
          for (k = 0, len = ref.length; k < len; k++) {
            o = ref[k];
            _this.elements[o.id] = new Ball(_this.options[Game.ElementTypes[o.type]], o);
            if (o.type === 0 && _this.player.balls.hasOwnProperty(o.id)) {
              _this.elements[o.id].options = _this.options.player;
              results.push(_this.player.balls[o.id] = _this.elements[o.id]);
            } else {
              results.push(void 0);
            }
          }
          return results;
        };
      })(this));
      this.net.on(Network.Packets.UpdateElements, (function(_this) {
        return function(packet) {
          var err, k, l, len, len1, len2, n, o, ref, ref1, ref2, results;
          try {
            ref = packet.newElements;
            for (k = 0, len = ref.length; k < len; k++) {
              o = ref[k];
              _this.elements[o.id] = new Ball(_this.options[Game.ElementTypes[o.type]], o);
              if (o.type === 0 && _this.player.balls.hasOwnProperty(o.id)) {
                _this.elements[o.id].options = _this.options.player;
                _this.player.balls[o.id] = _this.elements[o.id];
              }
            }
          } catch (_error) {
            err = _error;
          }
          try {
            ref1 = packet.deletedElements;
            for (l = 0, len1 = ref1.length; l < len1; l++) {
              o = ref1[l];
              delete _this.elements[o];
            }
          } catch (_error) {
            err = _error;
          }
          try {
            ref2 = packet.updateElements;
            results = [];
            for (n = 0, len2 = ref2.length; n < len2; n++) {
              o = ref2[n];
              results.push(_this.elements[o.id].updateData(o));
            }
            return results;
          } catch (_error) {
            err = _error;
          }
        };
      })(this));
      this.net.on(Network.Packets.PlayerUpdate, (function(_this) {
        return function(packet) {
          var b, k, len, ref;
          _this.player.mass = packet.mass;
          _this.player.balls = {};
          ref = packet.balls;
          for (k = 0, len = ref.length; k < len; k++) {
            b = ref[k];
            if (_this.elements.hasOwnProperty(b)) {
              _this.player.balls[b] = _this.elements[b];
              _this.elements[b].options = _this.options.player;
            } else {
              _this.player.balls[b] = null;
            }
          }
          _this.massText.innerHTML = "Mass: " + _this.player.mass;
          return _this.updatePlayer();
        };
      })(this));
      this.net.on(Network.Packets.RIP, (function(_this) {
        return function() {
          console.log("You got killed");
          _this.gameStarted = false;
          return spawnbox.hidden = false;
        };
      })(this));
      this.net.onDisconnect((function(_this) {
        return function() {
          console.log("Disconnected");
          return _this.inRoom = false;
        };
      })(this));
      this.net.on(Network.Packets.GetStats, (function(_this) {
        return function(packet) {
          return console.log("Stats", packet);
        };
      })(this));
      this.loop();
    }

    Game.prototype.init = function() {
      this.canvas = document.getElementById("cvs");
      this.canvas.addEventListener("mousemove", (function(_this) {
        return function(evt) {
          _this.target.x = evt.clientX - _this.screen.width / 2;
          _this.target.y = evt.clientY - _this.screen.height / 2;
          return _this.target.changed = true;
        };
      })(this));
      this.canvas.addEventListener("keypress", (function(_this) {
        return function(evt) {
          if (evt.charCode === 32) {
            _this.net.emit(Network.Packets.SplitUp);
          }
          if (evt.charCode === 119) {
            _this.net.emit(Network.Packets.Shoot);
          }
          if (evt.charCode === 100) {
            return _this.getStats();
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
        this.net.emit(Network.Packets.Leave);
        this.inRoom = false;
        setTimeout(((function(_this) {
          return function() {
            return _this.join(index);
          };
        })(this)), 10);
        return;
      }
      console.log("Connecting ...");
      this.net.emit(new JoinPacket(index));
      this.inRoom = true;
      this.lastRoom = index;
      return this.updatePlayer();
    };

    Game.prototype.start = function() {
      console.log("Starting Game");
      this.name = this.nameText.value;
      this.net.emit(new StartPacket(this.name));
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
          } else {
            _this.statusText.innerHTML = "FPS: -";
          }
          _this.lastTick = now;
          return _this.loop();
        };
      })(this));
    };

    Game.prototype.update = function(timediff) {
      var i, m, ref;
      ref = this.elements;
      for (i in ref) {
        m = ref[i];
        m.update(timediff);
      }
      if (this.gameStarted) {
        this.updatePlayer();
        if (this.target.changed) {
          this.net.emit(new TargetPacket(this.target.x, this.target.y));
          this.target.changed = false;
        }
      }
      this.fpstimer += timediff;
      this.fps++;
      if (this.fpstimer >= 1) {
        this.statusText.innerHTML = "FPS:" + this.fps;
        this.fps = 0;
        return this.fpstimer = 0;
      }
    };

    Game.prototype.updatePlayer = function() {
      var b, i, j, ref;
      if (this.gameStarted) {
        this.player.x = this.player.y = this.player.size = 0;
        i = 0;
        ref = this.player.balls;
        for (j in ref) {
          b = ref[j];
          if (!(b !== null)) {
            continue;
          }
          this.player.x += b.x * b.size;
          this.player.y += b.y * b.size;
          this.player.size += b.size;
          i++;
        }
        if (i > 0) {
          this.player.x /= this.player.size;
          this.player.y /= this.player.size;
          return this.player.size /= i;
        }
      } else {
        this.player.x = this.gamefield.width / 2;
        this.player.y = this.gamefield.height / 2;
        return this.player.size = Math.log(Math.min(this.gamefield.width - this.screen.width, this.gamefield.height - this.screen.height)) / this.options.viewPortScaleFactor / 2;
      }
    };

    Game.prototype.render = function() {
      var i, m, ref, results, scale, viewPort;
      scale = 1 / (this.options.viewPortScaleFactor * this.player.size + 1);
      this.graph.setTransform(scale, 0, 0, scale, this.screen.width / 2 - this.player.x * scale, this.screen.height / 2 - this.player.y * scale);
      viewPort = {
        left: this.player.x - this.screen.width / 2 / scale,
        top: this.player.y - this.screen.height / 2 / scale,
        right: this.player.x + this.screen.width / 2 / scale,
        botom: this.player.y + this.screen.height / 2 / scale
      };
      this.graph.fillStyle = this.options.backgroundColor;
      this.graph.fillRect(viewPort.left, viewPort.top, this.screen.width / scale, this.screen.height / scale);
      this.grid.render(this.graph);
      ref = this.elements;
      results = [];
      for (i in ref) {
        m = ref[i];
        if (m.x + m.size > viewPort.left && m.y + m.size > viewPort.top && m.x - m.size < viewPort.right && m.y - m.size < viewPort.botom) {
          results.push(m.render(this.graph));
        } else {
          results.push(void 0);
        }
      }
      return results;
    };

    Game.prototype.getStats = function() {
      this.net.emit(Network.Packets.GetStats);
    };

    Game.prototype.createRoom = function(name, options) {
      this.lobbySocket.emit("createRoom", name, options);
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
