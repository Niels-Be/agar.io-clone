//
// Created by niels on 02.06.15.
//

#ifndef AGARIO_GLOBALDEFS_H
#define AGARIO_GLOBALDEFS_H

#include <memory>
#include <functional>

using std::function;

#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>

using std::list;
using std::vector;
using std::map;
using std::unordered_map;

typedef std::string String;

#include <node.h>

#include <algorithm>
using std::max;
using std::min;

//#define min(a, b) ((a) < (b) ? (a) : (b))
//#define max(a, b) ((a) > (b) ? (a) : (b))
#define sign(x) (((x) > 0) ? 1 : (((x) < 0) ? -1 : 0))


class Packet;
typedef std::shared_ptr<Packet> PacketPtr;
class Client;
typedef std::shared_ptr<Client> ClientPtr;
class Server;
typedef std::shared_ptr<Server> ServerPtr;

class Gamefield;
typedef std::shared_ptr<Gamefield> GamefieldPtr;

class Player;
typedef std::shared_ptr<Player> PlayerPtr;

class Element;
typedef std::shared_ptr<Element> ElementPtr;
class Ball;
typedef std::shared_ptr<Ball> BallPtr;
class Shoot;
typedef std::shared_ptr<Shoot> ShootPtr;




#endif //AGARIO_GLOBALDEFS_H
