Agar.io Clone
=============

A Agar.io clone built with socket.io and HTML5 Canvas on top of NodeJS.

![Image](http://i.imgur.com/Ol8qtXb.jpg)

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
>You can check out a more detailed setup tutorial on @huytd`s [wiki](https://github.com/huytd/agar.io-clone/wiki/Setup)

#### Requirements
To run the game, you'll need: 
- NodeJS with NPM installed
- socket.io 
- Express


#### Downloading the Dependencies
After cloning the source code from Github, you need to run the following command to download all the dependencies (socket.io, express, etc.).

```
npm install
```

#### Running the Server
After download all the dependencies, you can run the server with the following command to run the server.

```
nodejs server/server.js
```

The game will then be accessible at `http://localhost:3000`.

