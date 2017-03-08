Agar.io Clone
=============

A Agar.io clone built with Websocket and HTML5 Canvas on top of a C++ Server.

![Image](http://fs1.directupload.net/images/150613/nrhpwl7u.png)

## Live Demo
[agar.waeco-soft.com](http://agar.waeco-soft.com/)

---
Tanks to [@huytd](https://github.com/huytd/agar.io-clone) for inspiration and parts of this readme

## How to Play
>You can check out a more elaborated how to play on @huytd`s [wiki](https://github.com/huytd/agar.io-clone/wiki/How-to-Play)

#### Game Basics
- Move your mouse on the screen to move yourself
- Eat food and other players in order to grow yourself (food respawns every time a player eats it)
- Player's **mass** is the number of food eaten
- Try to get fat and eat other players
- You can use your mass to split your self and get a speed boost (Key: Space)
- You can also use your mass to shoot out smaler pices (Key: W)

---

## Features
- Free name choose and random colors
- Splitting and shooting
- You can choose your room
  (Room creation is hardcoded at the moment)


#### Planed
- Color choose
- Items like boosts or bombs
- Obstracles
- Room creation by user

---

## Installation

#### Requirements
To build the Game you will need

##### Client
- NPM to install dependencies
- Grunt to compile coffeescript

##### Server
- CMake > 2.6
- C++11 Compiler
- [Websocketpp](https://github.com/zaphoyd/websocketpp)
- Boost-system library

#### Downloading the dependencies

Project: 

```
git clone https://github.com/WaeCo/agar.io-clone.git
cd agar.io-clone
sudo npm install -g grunt-cli
npm install
```

Install Server dependencies:

```
sudo apt-get install cmake, boost-all-dev
git clone https://github.com/zaphoyd/websocketpp.git
cd websocketpp
cmake .
sudo make install
```

## Building

### Client

```
grunt
```

### Server

```
cd server
cmake -DCMAKE_BUILD_TYPE=RELEASE .
make
```

---

## Running

Start the server

```
./server
```

The game will then be accessible at `http://localhost:3000`.
You will need a proxy that pass websocket calls to this port while delivering static resources to the client.

## Webserver Config

#### Nginx

```
server {
        listen 80 default_server;
        listen [::]:80 default_server ipv6only=on;

        root /path/to/agar.io-clone/client;
        index index.html index.htm;

        # Make site accessible from http://localhost/
        server_name localhost;

        location / {
                try_files $uri $uri/;
        }

        location /websocket/ {
                proxy_set_header Upgrade $http_upgrade;
                proxy_set_header Connection "upgrade";
                proxy_http_version 1.1;
                proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
                proxy_set_header Host $host;
                proxy_pass http://127.0.0.1:3000/;
        }

}

```

#### Apache 2.4

Make sure `mod_proxy`, `mod_proxy_http` and `mod_proxy_wstunnel` are enabled.


```
#Module dependencies
#  mod_proxy
#  mod_proxy_http
#  mod_proxy_wstunnel
<VirtualHost *:80>
  ServerName agar.example.com
  ServerSignature Off

  ProxyPreserveHost On

  # Ensure that encoded slashes are not decoded but left in their encoded state.
  # http://doc.gitlab.com/ce/api/projects.html#get-single-project
  AllowEncodedSlashes NoDecode

  <Location />
    Require all granted
  </Location>

  <Location /websocket/>
     ProxyPass ws://127.0.0.1:3000/
  </Location>

  DocumentRoot /path/to/agar.io-clone/client

  ErrorLog ${APACHE_LOG_DIR}/agar-error.log
  CustomLog ${APACHE_LOG_DIR}/agar-access.log combined

</VirtualHost>
```
