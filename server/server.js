var Ball, Element, Food, Gamefield, MoveableElement, Obstracle, Player, Shoot, StaticElement, app, express, extend, http, io, path, rooms, serveraddress, serverport, sign,
  extend1 = function(child, parent) { for (var key in parent) { if (hasProp.call(parent, key)) child[key] = parent[key]; } function ctor() { this.constructor = child; } ctor.prototype = parent.prototype; child.prototype = new ctor(); child.__super__ = parent.prototype; return child; },
  hasProp = {}.hasOwnProperty;

extend = exports.extend = function(object, properties) {
  var key, val;
  for (key in properties) {
    val = properties[key];
    object[key] = val;
  }
  return object;
};

sign = function(x) {
  if (x) {
    if (x < 0) {
      return -1;
    } else {
      return 1;
    }
  } else {
    return 0;
  }
};

Player = (function() {
  function Player(socket1, name1, color1, room) {
    this.socket = socket1;
    this.name = name1;
    this.color = color1;
    this.room = room;
    this.socket.on("splitUp", this.splitUp.bind(this));
    this.socket.on("shoot", this.shoot.bind(this));
    this.socket.on("updateTarget", this.setTarget.bind(this));
    this.balls = [this.room.createBall(this)];
    this.mass = 0;
  }

  Player.prototype.splitUp = function(target) {
    var b, ball, deg, j, len, ref, vel;
    deg = Math.atan2(target.y, target.x);
    vel = {
      x: this.room.options.shoot.speed * Math.cos(deg),
      y: this.room.options.shoot.speed * Math.sin(deg)
    };
    ref = this.balls;
    for (j = 0, len = ref.length; j < len; j++) {
      ball = ref[j];
      if (!(ball.mass >= this.room.options.player.minSpitMass)) {
        continue;
      }
      b = this.room.createBall(this);
      b.setMass(Math.floor(ball.mass / 2));
      ball.setMass(Math.floor(ball.mass / 2));
      b.x = ball.x + ball.size * 2 * Math.cos(deg);
      b.y = ball.y + ball.size * 2 * Math.sin(deg);
      b.setTarget(target);
      b.setBoost(vel, this.room.options.shoot.acceleration);
      this.balls.push(b);
    }
    return this.updateMass();
  };

  Player.prototype.shoot = function(target) {
    var b, ball, deg, j, len, ref;
    deg = Math.atan2(target.y, target.x);
    ref = this.balls;
    for (j = 0, len = ref.length; j < len; j++) {
      ball = ref[j];
      if (!(ball.mass > this.room.options.shoot.mass * 2)) {
        continue;
      }
      b = this.room.createShoot(ball);
      ball.addMass(-this.room.options.shoot.mass);
      b.x += ball.size * 2 * Math.cos(deg);
      b.y += ball.size * 2 * Math.sin(deg);
      b.setTarget(target);
    }
    return this.updateMass();
  };

  Player.prototype.setTarget = function(target) {
    var ball, j, len, ref, results, t;
    ref = this.balls;
    results = [];
    for (j = 0, len = ref.length; j < len; j++) {
      ball = ref[j];
      t = {
        x: target.x - (ball.x - this.x),
        y: target.y - (ball.y - this.y)
      };
      results.push(ball.setTarget(t));
    }
    return results;
  };

  Player.prototype.removeBall = function(ball) {
    var b, i, ref;
    ref = this.balls;
    for (i in ref) {
      b = ref[i];
      if (b.id === ball.id) {
        this.balls.splice(i, 1);
      }
    }
    if (this.balls.length === 0) {
      console.log("Player " + this.name + " got killed");
      this.socket.emit("rip");
    }
    return this.updateMass();
  };

  Player.prototype.update = function(timediff) {
    var ball, j, len, ref, results;
    this.x = this.y = 0;
    ref = this.balls;
    results = [];
    for (j = 0, len = ref.length; j < len; j++) {
      ball = ref[j];
      this.x += ball.x / this.balls.length;
      results.push(this.y += ball.y / this.balls.length);
    }
    return results;
  };

  Player.prototype.updateMass = function() {
    var ball, j, len, ref;
    this.mass = 0;
    ref = this.balls;
    for (j = 0, len = ref.length; j < len; j++) {
      ball = ref[j];
      this.mass += ball.mass;
    }
    return this.socket.emit("updatePlayer", this.get());
  };

  Player.prototype.get = function() {
    var b;
    return {
      name: this.name,
      mass: this.mass,
      balls: (function() {
        var j, len, ref, results;
        ref = this.balls;
        results = [];
        for (j = 0, len = ref.length; j < len; j++) {
          b = ref[j];
          results.push(b.id);
        }
        return results;
      }).call(this)
    };
  };

  return Player;

})();

Element = (function() {
  function Element(gamefield, x1, y1, color1, size) {
    this.gamefield = gamefield;
    this.x = x1;
    this.y = y1;
    this.color = color1;
    this.size = size != null ? size : 1;
    this.id = -1;
  }

  Element.prototype.intercept = function(other) {
    var distance;
    distance = (this.x - other.x) * (this.x - other.x) + (this.y - other.y) * (this.y - other.y);
    return distance <= (this.size * this.size);
  };

  Element.prototype.canEat = function(other) {
    return false;
  };

  Element.prototype.get = function() {
    return {
      id: this.id,
      x: this.x,
      y: this.y,
      color: this.color,
      size: this.size
    };
  };

  return Element;

})();

StaticElement = (function(superClass) {
  extend1(StaticElement, superClass);

  function StaticElement() {
    return StaticElement.__super__.constructor.apply(this, arguments);
  }

  StaticElement.prototype.update = function(t) {};

  return StaticElement;

})(Element);

MoveableElement = (function(superClass) {
  extend1(MoveableElement, superClass);

  function MoveableElement(gamefield, x1, y1, color1, size, speed1) {
    this.gamefield = gamefield;
    this.x = x1;
    this.y = y1;
    this.color = color1;
    this.size = size != null ? size : 1;
    this.speed = speed1 != null ? speed1 : 100;
    this.velocity = {
      x: 0,
      y: 0
    };
    this.boostVel = {
      x: 0,
      y: 0
    };
    this.boostAcc = 0;
    this.target = {
      x: 0,
      y: 0
    };
  }

  MoveableElement.prototype.setBoost = function(boostVel, boostAcc) {
    this.boostVel = boostVel;
    this.boostAcc = boostAcc;
  };

  MoveableElement.prototype.setTarget = function(target1) {
    this.target = target1;
  };

  MoveableElement.prototype.update = function(t) {
    var a, ba, deg, degX, degY, deltaX, deltaY, ref, ref1, velX, velY;
    if (this.boostVel.x !== 0 || this.boostVel.y !== 0) {
      ba = -this.boostAcc;
      velX = ba * t * sign(this.boostVel.x) + this.boostVel.x;
      velY = ba * t * sign(this.boostVel.y) + this.boostVel.y;
      this.boostVel.x = sign(velX) === sign(this.boostVel.x) ? velX : 0;
      this.boostVel.y = sign(velY) === sign(this.boostVel.y) ? velY : 0;
    }
    a = this.gamefield.options.player.acceleration;
    if (this.target.x !== 0 || this.target.y !== 0) {
      deg = Math.atan2(this.target.y, this.target.x);
      degX = Math.cos(deg);
      degY = Math.sin(deg);
      velX = a * t * degX + this.velocity.x;
      velY = a * t * degY + this.velocity.y;
      this.velocity.x = Math.abs(velX) > Math.abs(this.speed * degX) ? this.speed * degX : velX;
      this.velocity.y = Math.abs(velY) > Math.abs(this.speed * degY) ? this.speed * degY : velY;
    } else {
      velX = this.velocity.x - a * t * sign(this.velocity.x);
      velY = this.velocity.y - a * t * sign(this.velocity.y);
      this.velocity.x = sign(velX) === sign(this.velocity.x) ? velX : 0;
      this.velocity.y = sign(velY) === sign(this.velocity.y) ? velY : 0;
    }
    deltaX = (this.boostVel.x + this.velocity.x) * t;
    deltaY = (this.boostVel.y + this.velocity.y) * t;
    if ((0 <= (ref = this.x + deltaX) && ref <= this.gamefield.options.width)) {
      this.x += deltaX;
    }
    if ((0 <= (ref1 = this.y + deltaY) && ref1 <= this.gamefield.options.height)) {
      this.y += deltaY;
    }
    if (this.x < 0 && deltaX > 0 || this.x > this.gamefield.options.width && deltaX < 0) {
      this.x += deltaX;
    }
    if (this.y < 0 && deltaY > 0 || this.y > this.gamefield.options.height && deltaY < 0) {
      return this.y += deltaY;
    }
  };

  MoveableElement.prototype.get = function() {
    var vel;
    vel = {
      x: this.boostVel.x + this.velocity.x,
      y: this.boostVel.y + this.velocity.y
    };
    return extend(MoveableElement.__super__.get.call(this), {
      velocity: vel
    });
  };

  return MoveableElement;

})(Element);

Gamefield = (function() {
  Gamefield.defaultOptions = {
    width: 5000,
    height: 5000,
    food: {
      color: '#f1c40f',
      spawn: 5,
      max: 500,
      mass: 1,
      size: 5
    },
    player: {
      color: ['#EA6153', '#00FFFF', '#7FFF00', '#6495ED', '#9932CC', '#FF00FF', '#FFE4B5', '#000000'],
      size: 15,
      force: 50,
      acceleration: 1000,
      speed: 300,
      speedPenalty: 0.005,
      eatFactor: 1.2,
      minSpitMass: 20
    },
    shoot: {
      mass: 10,
      speed: 750,
      acceleration: 400
    }
  };

  function Gamefield(name1, options) {
    var j, ref;
    this.name = name1;
    this.options = extend(extend({}, Gamefield.defaultOptions), options);
    this.room = io.of('/' + this.name);
    console.log("Created Room " + this.name);
    this.player = {
      length: 0
    };
    this.elements = {
      "static": [],
      moveable: [],
      id: 1
    };
    this.foodSpawnTimer = 0;
    this.foodCount = 0;
    this.playerUpdateTimer = process.hrtime();
    this.timerMoveables = [];
    this.timerCollision = [];
    this.timerOther = [];
    this.updaterStarted = 0;
    for (j = 0, ref = this.options.food.max / 2; 0 <= ref ? j <= ref : j >= ref; 0 <= ref ? j++ : j--) {
      this.createFood();
    }
    this.room.on("connection", (function(_this) {
      return function(socket) {
        console.log("Client connected to room " + _this.name);
        _this.updaterStarted++;
        if (_this.updaterStarted === 1) {
          _this.update(0);
        }
        socket.on("join", function() {
          var s;
          console.log("Client is ready");
          socket.emit("updateMoveables", (function() {
            var k, len, ref1, results;
            ref1 = this.elements.moveable;
            results = [];
            for (k = 0, len = ref1.length; k < len; k++) {
              s = ref1[k];
              results.push(s.get());
            }
            return results;
          }).call(_this));
          return socket.emit("updateStatics", (function() {
            var k, len, ref1, results;
            ref1 = this.elements["static"];
            results = [];
            for (k = 0, len = ref1.length; k < len; k++) {
              s = ref1[k];
              results.push(s.get());
            }
            return results;
          }).call(_this));
        });
        socket.on("leave", function() {
          console.log("Client left room " + _this.name);
          return socket.disconnect();
        });
        socket.on("start", function(name) {
          var color, ply;
          console.log("Player " + name + " joind the game " + _this.name);
          color = _this.options.player.color[Math.round(Math.random() * _this.options.player.color.length)];
          ply = new Player(socket, name, color, _this);
          _this.player[socket.id] = ply;
          _this.player.length++;
          _this.room.emit("playerJoined", ply.get());
          return socket.emit("start", ply.get());
        });
        socket.on("disconnect", function() {
          var b, e, i, k, len, ref1, ref2;
          _this.updaterStarted--;
          if (_this.player.hasOwnProperty(socket.id)) {
            console.log("Player " + _this.player[socket.id].name + " disconnected");
            ref1 = _this.player[socket.id].balls;
            for (k = 0, len = ref1.length; k < len; k++) {
              b = ref1[k];
              ref2 = _this.elements.moveable;
              for (i in ref2) {
                e = ref2[i];
                if (e.id === b.id) {
                  _this.elements.moveable.splice(i, 1);
                }
              }
            }
            delete _this.player[socket.id];
            return _this.player.length--;
          }
        });
        return socket.on("getStats", function() {
          var c, k, l, len, len1, len2, m, n, o, ref1, ref2, ref3, tm;
          m = c = o = 0;
          ref1 = _this.timerMoveables;
          for (k = 0, len = ref1.length; k < len; k++) {
            tm = ref1[k];
            m += tm;
          }
          ref2 = _this.timerCollision;
          for (l = 0, len1 = ref2.length; l < len1; l++) {
            tm = ref2[l];
            c += tm;
          }
          ref3 = _this.timerOther;
          for (n = 0, len2 = ref3.length; n < len2; n++) {
            tm = ref3[n];
            o += tm;
          }
          console.log(m, c, o);
          return socket.emit("stats", {
            moveables: m / _this.timerMoveables.length,
            collision: c / _this.timerCollision.length,
            other: o / _this.timerOther.length
          });
        });
      };
    })(this));
  }

  Gamefield.prototype.generatePos = function() {
    return [Math.floor(Math.random() * this.options.width), Math.floor(Math.random() * this.options.height)];
  };

  Gamefield.prototype.createFood = function() {
    var f, ref, x, y;
    this.foodCount++;
    ref = this.generatePos(), x = ref[0], y = ref[1];
    f = new Food(this, x, y);
    f.id = this.elements.id++;
    this.elements["static"].push(f);
    return f;
  };

  Gamefield.prototype.createBall = function(player) {
    var b, ref, x, y;
    ref = this.generatePos(), x = ref[0], y = ref[1];
    b = new Ball(this, x, y, player.color, player, this.options.player.size, this.options.player.speed);
    b.id = this.elements.id++;
    this.elements.moveable.push(b);
    return b;
  };

  Gamefield.prototype.createShoot = function(ball) {
    var b;
    b = new Shoot(this, ball.x, ball.y, ball.color);
    b.id = this.elements.id++;
    this.elements.moveable.push(b);
    return b;
  };

  Gamefield.prototype.destroyElement = function(elem) {
    var e, i, ref, ref1;
    if (elem instanceof StaticElement) {
      if (elem instanceof Food) {
        this.foodCount--;
      }
      ref = this.elements["static"];
      for (i in ref) {
        e = ref[i];
        if (e.id === elem.id) {
          this.elements["static"].splice(i, 1);
        }
      }
    } else if (elem instanceof MoveableElement) {
      if (elem.player && elem instanceof Ball) {
        elem.player.removeBall(elem);
      }
      ref1 = this.elements.moveable;
      for (i in ref1) {
        e = ref1[i];
        if (e.id === elem.id) {
          this.elements.moveable.splice(i, 1);
        }
      }
    } else {
      throw "Element is not of type Element";
    }
    return this.room.emit("deleteElement", elem.id);
  };

  Gamefield.prototype.update = function(timediff) {
    var destoryLater, diff, e, elem, elem1, elem2, f, i, id, j, k, l, len, len1, len2, len3, m, n, ply, ref, ref1, ref2, ref3, ref4, sleep, timerCollision, timerMoveables, timerOther;
    this.playerUpdateTimer = process.hrtime();
    if (this.updaterStarted === 0) {
      return;
    }
    ref = this.elements.moveable;
    for (j = 0, len = ref.length; j < len; j++) {
      elem = ref[j];
      elem.update(timediff);
    }
    timerMoveables = process.hrtime(this.playerUpdateTimer);
    timerMoveables = timerMoveables[0] * 1e3 + timerMoveables[1] * 1e-6;
    destoryLater = [];
    ref1 = this.elements.moveable;
    for (i in ref1) {
      elem1 = ref1[i];
      ref2 = this.elements["static"];
      for (k = 0, len1 = ref2.length; k < len1; k++) {
        elem2 = ref2[k];
        if (elem1.intercept(elem2)) {
          if (elem1.canEat(elem2)) {
            elem1.addMass(elem2.mass);
            this.destroyElement(elem2);
            if (elem1.player) {
              elem1.player.updateMass();
            }
            break;
          }
        }
      }
      ref3 = this.elements.moveable;
      for (l = 0, len2 = ref3.length; l < len2; l++) {
        elem2 = ref3[l];
        if (elem1.id !== elem2.id && elem1.intercept(elem2)) {
          if (elem1.canEat(elem2)) {
            elem1.addMass(elem2.mass);
            destoryLater.push(elem2);
            if (elem1.player) {
              elem1.player.updateMass();
            }
          } else if (elem2.canEat(elem1)) {
            elem2.addMass(elem1.mass);
            destoryLater.push(elem1);
            if (elem2.player) {
              elem2.player.updateMass();
            }
          }
        }
      }
    }
    for (n = 0, len3 = destoryLater.length; n < len3; n++) {
      e = destoryLater[n];
      this.destroyElement(e);
    }
    timerCollision = process.hrtime(this.playerUpdateTimer);
    timerCollision = timerCollision[0] * 1e3 + timerCollision[1] * 1e-6 - timerMoveables;
    ref4 = this.player;
    for (id in ref4) {
      ply = ref4[id];
      if (id !== "length") {
        ply.update(timediff);
      }
    }
    this.foodSpawnTimer += timediff;
    if (this.foodSpawnTimer > 1 / this.options.food.spawn) {
      if (this.foodCount < this.options.food.max) {
        f = this.createFood();
        this.room.emit("createFood", f.get());
      }
      this.foodSpawnTimer = 0;
    }
    this.room.emit("updateMoveables", (function() {
      var len4, p, ref5, results;
      ref5 = this.elements.moveable;
      results = [];
      for (p = 0, len4 = ref5.length; p < len4; p++) {
        m = ref5[p];
        results.push(m.get());
      }
      return results;
    }).call(this));
    diff = process.hrtime(this.playerUpdateTimer);
    sleep = 1000 / 60 - (diff[0] * 1e3 + diff[1] * 1e-6);
    if (sleep >= 1) {
      setTimeout((function(_this) {
        return function() {
          var diff2;
          diff2 = process.hrtime(_this.playerUpdateTimer);
          return _this.update(diff2[0] + diff2[1] * 1e-9);
        };
      })(this), sleep);
    } else {
      setImmediate(this.update.bind(this, diff[0] + diff[1] * 1e-9));
    }
    timerOther = process.hrtime(this.playerUpdateTimer);
    timerOther = timerOther[0] * 1e3 + timerOther[1] * 1e-6 - timerCollision - timerMoveables;
    this.timerMoveables.unshift(timerMoveables);
    if (!(this.timerMoveables.length > 1000)) {
      this.timerMoveables.pop;
    }
    this.timerCollision.unshift(timerCollision);
    if (!(this.timerCollision.length > 1000)) {
      this.timerCollision.pop;
    }
    this.timerOther.unshift(timerOther);
    if (!(this.timerOther.length > 1000)) {
      return this.timerOther.pop;
    }
  };

  Gamefield.prototype.get = function() {
    return {
      name: this.name,
      playercount: this.player.length,
      options: this.options
    };
  };

  return Gamefield;

})();

Food = (function(superClass) {
  extend1(Food, superClass);

  function Food(gamefield, x1, y1) {
    this.gamefield = gamefield;
    this.x = x1;
    this.y = y1;
    this.color = this.gamefield.options.food.color;
    this.mass = this.gamefield.options.food.mass;
    this.size = this.gamefield.options.food.size;
  }

  Food.prototype.get = function() {
    return extend(Food.__super__.get.call(this), {
      type: "food"
    });
  };

  return Food;

})(StaticElement);

Obstracle = (function(superClass) {
  extend1(Obstracle, superClass);

  function Obstracle(gamefield, x1, y1, color1, size) {
    this.gamefield = gamefield;
    this.x = x1;
    this.y = y1;
    this.color = color1;
    this.size = size;
  }

  Obstracle.prototype.get = function() {
    return extend(Obstracle.__super__.get.call(this), {
      type: "Obstracle"
    });
  };

  return Obstracle;

})(StaticElement);

Ball = (function(superClass) {
  extend1(Ball, superClass);

  function Ball(gamefield, x1, y1, color1, player1, size, speed1) {
    this.gamefield = gamefield;
    this.x = x1;
    this.y = y1;
    this.color = color1;
    this.player = player1;
    this.size = size != null ? size : 1;
    this.speed = speed1 != null ? speed1 : 100;
    Ball.__super__.constructor.call(this, this.gamefield, this.x, this.y, this.color, this.size, this.speed);
    this.setMass(20);
  }

  Ball.prototype.setMass = function(mass) {
    this.mass = mass;
    this.size = this.gamefield.options.player.size + 150 * Math.log((mass + 150) / 150);
    return this.speed = this.gamefield.options.player.speed * Math.exp(-this.gamefield.options.player.speedPenalty * mass);
  };

  Ball.prototype.addMass = function(mass) {
    return this.setMass(this.mass + mass);
  };

  Ball.prototype.canEat = function(other) {
    return other.size * this.gamefield.options.player.eatFactor < this.size;
  };

  Ball.prototype.get = function() {
    return extend(Ball.__super__.get.call(this), {
      name: this.player ? this.player.name : void 0,
      mass: this.mass,
      type: "ball"
    });
  };

  return Ball;

})(MoveableElement);

Shoot = (function(superClass) {
  extend1(Shoot, superClass);

  function Shoot(gamefield, x1, y1, color1) {
    this.gamefield = gamefield;
    this.x = x1;
    this.y = y1;
    this.color = color1;
    Shoot.__super__.constructor.call(this, this.gamefield, this.x, this.y, this.color);
    this.speed = this.gamefield.options.shoot.speed;
    this.setMass(this.gamefield.options.shoot.mass);
  }

  Shoot.prototype.setTarget = function(target) {
    var deg, vel;
    deg = Math.atan2(target.y, target.x);
    vel = {
      x: this.speed * Math.cos(deg),
      y: this.speed * Math.sin(deg)
    };
    return this.setBoost(vel, this.gamefield.options.shoot.acceleration);
  };

  Shoot.prototype.setMass = function(mass) {
    var speed;
    speed = this.speed;
    Shoot.__super__.setMass.call(this, mass);
    return this.speed = speed;
  };

  Shoot.prototype.get = function() {
    return extend(Shoot.__super__.get.call(this), {
      type: "shoot"
    });
  };

  return Shoot;

})(Ball);

path = require('path');

express = require('express');

app = express();

http = require('http').Server(app);

io = require('socket.io')(http);

app.use(express["static"]('client'));

rooms = {};

rooms["default"] = new Gamefield("default");

rooms["small"] = new Gamefield("small", {
  width: 2000,
  height: 2000
});

io.on("connection", (function(_this) {
  return function(socket) {
    console.log("Got new Server connection");
    socket.on("getRooms", function() {
      var r;
      return socket.emit("availableRooms", (function() {
        var results;
        results = [];
        for (r in rooms) {
          results.push(rooms[r].get());
        }
        return results;
      })());
    });
    socket.on("createRoom", function(name, options) {
      if (!rooms.hasOwnProperty(name)) {
        return rooms[name] = new Gamefield(name, options);
      } else {
        return socket.emit("roomCreateFailed", "Name already exists");
      }
    });
    return socket.on("error", function(err) {
      return console.log("Error:", err);
    });
  };
})(this));

serveraddress = process.env.OPENSHIFT_NODEJS_IP || process.env.IP || '127.0.0.1';

serverport = process.env.OPENSHIFT_NODEJS_PORT || process.env.PORT || 3000;

http.listen(serverport, serveraddress, (function(_this) {
  return function() {
    return console.log('Server listening on ' + serveraddress + ':' + serverport);
  };
})(this));
