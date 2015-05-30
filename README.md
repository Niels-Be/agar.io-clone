Agar.io Clone
=============

A Agar.io clone built with socket.io and HTML5 Canvas on top of NodeJS.

![Image](http://i.imgur.com/Ol8qtXb.jpg)

## Live Demo
[agar.waeco-soft.com](http://agar.waeco-soft.com/)

---
Tanks to [@huytd](https://github.com/huytd/agar.io-clone) for inspiration and parts of this readme

## How to Play
>You can check out a more elaborated how to play on our [wiki](https://github.com/huytd/agar.io-clone/wiki/How-to-Play)

#### Game Basics
- Move your mouse on the screen to move yourself
- Eat food and other players in order to grow yourself (food respawns every time a player eats it)
- Player's **mass** is the number of food eaten
- Try to get fat and eat other players

#### Gameplay Rules
- Players who haven't eaten yet can't be eaten
- Everytime a player joins the game, **3** foods will be spawned
- Everytime a food is eaten by a player, **1** new food will be respawned
- The more food you eat, the slower you move

---

## Installation
>You can check out a more detailed setup tutorial on our [wiki](https://github.com/huytd/agar.io-clone/wiki/Setup)

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

