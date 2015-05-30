var playerName = prompt("Your name please:").replace(/(<([^>]+)>)/ig,"");

// Canvas
var screenWidth = window.innerWidth;
var screenHeight = window.innerHeight;
var viewWidth = screenWidth;
var viewHeight = screenHeight;

var socket = io();
var gameStart = false;
var disconnected = false;

var startPingTime = 0;

var KEY_ENTER = 13;

var chatCommands = {};
var backgroundColor = "#EEEEEE";

var foodConfig = {
  border: 2,
  borderColor: "#f39c12",
  fillColor: "#f1c40f",
  size: 5
};

var playerConfig = {
  border: 3,
  borderColor: "#c0392b",
  fillColor: "#ea6153",
  textColor: "#FFFFFF",
  textBorder: "#000000",
  textBorderSize: 3,
  defaultSize: 10
};

var enemyConfig = {
  border: 3,
  borderColor: "#27ae60",
  fillColor: "#2ecc71",
  textColor: "#FFFFFF",
  textBorder: "#000000",
  textBorderSize: 3,
  defaultSize: 10
};

var settings = {};

var gridSize = 50;
var viewPortScaleFactor = 0.01;

var player = {
  id: -1,
  x: screenWidth / 2, y: screenHeight / 2,
  mass: 0, speed: 200,
};

var foods = [];
var enemies = [];
var target = {x: player.x, y: player.y};

var c = document.getElementById("cvs");
c.addEventListener("mousemove", gameMouseInput, false);
c.addEventListener("keypress", gameKeyInput, false);
c.width = screenWidth; 
c.height = screenHeight;

window.addEventListener("resize", function() {
  screenWidth = window.innerWidth;
  screenHeight = window.innerHeight;
  c.width = screenWidth; 
  c.height = screenHeight;
});

var graph = c.getContext("2d");

var chatInput = document.getElementById("chatInput");
chatInput.addEventListener("keypress", sendChat);

// Chat
function addChatLine(name, text) {
  var chatLine = document.createElement("li");
  chatLine.className = (name == player.name)?"me":"friend";
  chatLine.innerHTML = "<b>" + name + "</b>: " + text;
  var chatList = document.getElementById("chatList");
  chatList.insertBefore(chatLine, chatList.childNodes[0]);
}

function addSystemLine(text) {
  var chatLine = document.createElement("li");
  chatLine.className = "system";
  chatLine.innerHTML = text;
  var chatList = document.getElementById("chatList");
  chatList.insertBefore(chatLine, chatList.childNodes[0]);
}

function registerChatCommand(name, description, callback) {
  chatCommands[name] = {
    description: description,
    callback: callback
  };
}

function checkLatency() {
  // Ping
  startPingTime = Date.now();
  socket.emit("ping");
}

function toggleDarkMode(args) {
  var LIGHT = '#EEEEEE';
  var DARK = '#181818';
  var on = args[0] === 'on';
  var off = args[0] === 'off';

  if (on || (!off && backgroundColor === LIGHT)) {
    backgroundColor = DARK;
    addSystemLine('Dark mode enabled');
  } else {
    backgroundColor = LIGHT;
    addSystemLine('Dark mode disabled');
  }
}

function printHelp() {
  for (var command in chatCommands) {
    if (chatCommands.hasOwnProperty(command)) {
      addSystemLine('-' + command + ': ' + chatCommands[command].description);
    }
  }
}

registerChatCommand('ping', 'check your latency', function () {
  checkLatency();
});

registerChatCommand('dark', 'toggle dark mode', function (args) {
  toggleDarkMode(args);
});

registerChatCommand('help', 'show information about chat commands', function () {
  printHelp();
});

function sendChat(key) {
  key = key.which || key.keyCode;
  if (key == KEY_ENTER) {
    var text = chatInput.value.replace(/(<([^>]+)>)/ig,"");
    if (text !== "") {
      if (text.indexOf('-') === 0) {
        var args = text.substring(1).split(' ');
        if (chatCommands[args[0]]) {
          chatCommands[args[0]].callback(args.slice(1));
        } else {
          addSystemLine('Unrecoginised Command: ' + text + ', type -help for more info');
        }
      } else {
        socket.emit("playerChat", { sender: player.name, message: text });
        addChatLine(player.name, text);
      }
      chatInput.value = "";
    }
  }
}

// Handle ping
socket.on("pong", function(){
  var latency = Date.now() - startPingTime;
  console.log("Latency: " + latency + "ms");
  addSystemLine("Ping: " + latency + "ms");
});

// Handle error
socket.on("connect_failed", function() {
  socket.close();
  disconnected = true;
});

socket.on("disconnect", function() {
  socket.close();
  disconnected = true;
});

// Handle connection
socket.on("welcome", function(userID, serverSettings) {
  settings = serverSettings;
  player.id = userID;
  player.name = playerName;
  socket.emit("gotit", player);
  gameStart = true;
  console.log("Game is started: " + gameStart);
  addSystemLine("Connected to the game!");
});

socket.on("playerDisconnect", function(data) {
  enemies = data.playersList;
  document.getElementById("status").innerHTML = "Players: " + enemies.length;
  addSystemLine("Player <b>" + data.disconnectName + "</b> disconnected!");
});

socket.on("playerJoin", function(data) {
  enemies = data.playersList;
  document.getElementById("status").innerHTML = "Players: " + enemies.length;
  addSystemLine("Player <b>" + data.connectedName + "</b> joined!");
});

// Chat
socket.on("serverSendPlayerChat", function(data){
  addChatLine(data.sender, data.message);
});

// Handle movement
socket.on("serverTellPlayerMove", function(playerData) {
  player = playerData;
  document.getElementById("mass").innerHTML = "Mass: " + player.mass;
});

socket.on("serverUpdateAllPlayers", function(players) {
  enemies = players;
});


// Update others
socket.on("serverTellPlayerUpdateFoods", function(foodsList) {
  foods = foodsList;
});

socket.on("serverUpdateAllFoods", function(foodsList) {
  foods = foodsList;
});


// Die
socket.on("RIP", function(){
  gameStart = false;
  socket.close();
  alert("You got killed");
});

socket.on("error", function(err) {
  console.log("Error:", err);
});

function drawGrid() {
  var px = player.x % gridSize;
  var py = player.y % gridSize;
  
  graph.beginPath();
  graph.lineWidth = 1;
  graph.strokeStyle = '#DADADA';
  for(var x = 0; x <= viewWidth; x += gridSize) {
    graph.moveTo(0 + x - px, 0);
    graph.lineTo(0 + x - px, viewHeight);
  }
  for(var y = 0; y <= viewHeight; y += gridSize) {
    graph.moveTo(0, 0 + y - py);
    graph.lineTo(viewWidth, 0 + y - py);
  }
  graph.stroke();

  graph.beginPath();
  graph.lineWidth = 5;
  graph.strokeStyle = '#A0A0A0'
  graph.moveTo(viewWidth/2 - player.x, 0);
  graph.lineTo(viewWidth/2 - player.x, viewHeight);

  graph.moveTo(viewWidth/2 - player.x + settings.gamefieldSize, 0);
  graph.lineTo(viewWidth/2 - player.x + settings.gamefieldSize, viewHeight);

  graph.moveTo(0, viewHeight/2 - player.y + settings.gamefieldSize);
  graph.lineTo(viewWidth, viewHeight/2 - player.y + settings.gamefieldSize);

  graph.moveTo(0, viewHeight/2 - player.y);
  graph.lineTo(viewWidth, viewHeight/2 - player.y);

  graph.stroke();
}

function drawFood(food) {
  graph.strokeStyle = foodConfig.borderColor;
  graph.fillStyle = foodConfig.fillColor;
  graph.lineWidth = foodConfig.border;
  graph.beginPath();
  graph.arc(food.x - player.x + viewWidth / 2, food.y - player.y + viewHeight / 2, foodConfig.size, 0, 2 * Math.PI);
  graph.stroke();
  graph.fill();
}

function drawPlayer() {
  graph.strokeStyle = playerConfig.borderColor;
  graph.fillStyle = playerConfig.fillColor;
  graph.lineWidth = playerConfig.border;
  graph.beginPath();
  graph.arc(viewWidth / 2, viewHeight / 2, playerConfig.defaultSize + player.mass, 0, 2 * Math.PI);
  graph.stroke();
  graph.fill();

  var fontSize = (player.mass / 2) + playerConfig.defaultSize;
  graph.lineWidth = playerConfig.textBorderSize;
  graph.textAlign = "center";
  graph.fillStyle = playerConfig.textColor;
  graph.textBaseline = 'middle';
  graph.strokeStyle = playerConfig.textBorder;
  graph.font = "bold " + fontSize + "px sans-serif";
  graph.strokeText(player.name, viewWidth / 2, viewHeight / 2);
  graph.fillText(player.name, viewWidth / 2, viewHeight / 2);
}

function drawEnemy(enemy) {
  graph.strokeStyle = enemyConfig.borderColor;
  graph.fillStyle = enemyConfig.fillColor;
  graph.lineWidth = enemyConfig.border;
  graph.beginPath();
  graph.arc(enemy.x - player.x + viewWidth / 2, enemy.y - player.y + viewHeight / 2, enemyConfig.defaultSize + enemy.mass, 0, 2 * Math.PI);
  graph.fill();
  graph.stroke();

  var fontSize = (enemy.mass / 2) + enemyConfig.defaultSize;

  graph.lineWidth = enemyConfig.textBorderSize;
  graph.textAlign = "center";
  graph.fillStyle = enemyConfig.textColor;
  graph.textBaseline = 'middle';
  graph.strokeStyle = enemyConfig.textBorder;
  graph.font = "bold " + fontSize + "px sans-serif";
  graph.strokeText(enemy.name, enemy.x - player.x + viewWidth / 2, enemy.y - player.y + viewHeight / 2);
  graph.fillText(enemy.name, enemy.x - player.x + viewWidth / 2, enemy.y - player.y + viewHeight / 2);
}

function gameMouseInput(mouse) {
  target.x = mouse.clientX - screenWidth  / 2;
  target.y = mouse.clientY - screenHeight / 2;
}

function gameKeyInput(key) {
  if(key.keyCode == 119) {
    socket.emit('playerSplitUp', target);
  }
}

window.requestAnimFrame = (function(){
  return  window.requestAnimationFrame       ||
          window.webkitRequestAnimationFrame ||
          window.mozRequestAnimationFrame    ||
          function( callback ){
            window.setTimeout(callback, 1000 / 60);
          };
})();

(function animloop(){
  requestAnimFrame(animloop);
  gameLoop();
})();

function gameLoop() {
  if (!disconnected) {
    if (gameStart) {
      graph.setTransform(1,0,0,1,0,0);
      graph.fillStyle = backgroundColor;
      graph.fillRect(0, 0, screenWidth, screenHeight);

      scale = viewPortScaleFactor * player.mass + 1;
      graph.setTransform(1/scale,0,0,1/scale,0,0);
      viewWidth  = screenWidth  * scale;
      viewHeight = screenHeight * scale;
      drawGrid();

      for (var i = 0; i < foods.length; i++) {
        drawFood(foods[i]);
      }

      for (i = 0; i < enemies.length; i++) {
        if (enemies[i].id != player.id) {
          drawEnemy(enemies[i]);
        }
      }

      drawPlayer();
      socket.emit("playerSendTarget", target);
    } else {
      graph.setTransform(1,0,0,1,0,0);
      graph.fillStyle = "#333333";
      graph.fillRect(0, 0, screenWidth, screenHeight);

      graph.textAlign = "center";
      graph.fillStyle = "#FFFFFF";
      graph.font = "bold 30px sans-serif";
      graph.fillText("Game Over!", screenWidth / 2, screenHeight / 2);
    }
  } else {
    graph.setTransform(1,0,0,1,0,0);
    graph.fillStyle = "#333333";
    graph.fillRect(0, 0, screenWidth, screenHeight);

    graph.textAlign = "center";
    graph.fillStyle = "#FFFFFF";
    graph.font = "bold 30px sans-serif";
    graph.fillText("Disconnected!", screenWidth / 2, screenHeight / 2);
  }
}
