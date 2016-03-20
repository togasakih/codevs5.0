#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <set>
#include <cassert>
#include <map>
#include <tuple>
using namespace std;
//global

const int INF = 255;

const char CELL_EMPTY = '_';
const char CELL_WALL ='W';
const char CELL_OBJECT = 'O';

const int dx[] =    {  0,   1,   0,  -1,   0};
const int dy[] =    { -1,   0,   1,   0,   0};
const string ds[] = {"U", "R", "D", "L",  ""};
const int dogDx[] =    {  0,   -1,   1,  0,   0};//U L R D
const int dogDy[] =    { -1,   0,   0,   1,   0};
const int cornerX[] = {1, 12, 1, 12};
const int cornerY[] = {1, 1, 15, 15};
const int pow5[] = {1, 5, 25, 125, 625, 3125, 15625};
const int HEIGHT = 17;
const int WIDTH = 14;

class Point {
public:
  int x, y;
  Point() { x = y = -1; }
  Point(int x, int y): x(x), y(y) {}
  bool operator== (const Point &p) const { return x == p.x && y == p.y; }
  bool operator < (const Point &p) const {
    if (x == p.x){
      return y < p.y;
    }
    return x < p.x;
  }
};

class Skill {
public:
  int id, cost;
  Skill() { id = cost = -1; }
  Skill(int id, int cost): id(id), cost(cost) {}

  static Skill input(int id) {
    int cost;
    cin >> cost;
    return Skill(id, cost);
  }
};

class Cell : public Point {
public:
  char kind;
  bool containsNinja, containsSoul, containsDog;
  Cell() {
    kind = '?';
    containsNinja = containsSoul = containsDog = false;
  }
  Cell(int x, int y, char kind): Point(x, y), kind(kind) {
    containsNinja = containsSoul = containsDog = false;
  }

  bool isWall() const { return kind == CELL_WALL; }
  bool isObject() const { return kind == CELL_OBJECT; }
  bool isEmpty() const { return kind == CELL_EMPTY; }
  //togasaki
  bool isNoCharacter() const {
    return !containsSoul & !containsNinja & !containsDog;
  }
};


vector<Skill> skills;
class Character : public Point {
public:
  int id;
  Character() { id = -1; }
  Character(int id, int x, int y): id(id), Point(x, y) {}

  static Character input() {
    int id, x, y;
    cin >> id >> y >> x;
    return Character(id, x, y);
  }
};

class State {
public:
  int skillPoint;
  int H, W;
  vector< vector<Cell> > field;
  vector<Character> ninjas;
  vector<Character> dogs;
  vector<Point> souls;
  //  vector<int> skillCount;
  //togasaki
  int getSoul;
  //  bool fail;
  int commandId;
  vector<int> minDistSoulById;
  int rivalSkillPoint;
  
  int skillUseId;
  int skillId;
  Point targetPoint;
  int skillNumOfUse;
  int skillRivaUselId;
  int skillRivalId;
  Point targetRivalPoint;
  
  vector<int> survive;
  int manhattanDistance;

  int preNinjaX0;
  int preNinjaY0;

  int preNinjaX1;
  int preNinjaY1;
  int manhattanPreDistance;
  int killDog;
  vector<int> minSoulManhattanDistance;
  bool ninjaConfined;


  //attack phase
  int kill;
  bool attackMode;
  int cornerClosed;
  int surroundNumOfDog;
  State() {
    skillPoint = H = W = -1;
    field.clear();
    ninjas.clear();
    dogs.clear();
    souls.clear();
    //    skillCount.clear();
    //togasaki

    getSoul = 0;
    //    fail = false;
    commandId = -1;
    minDistSoulById.clear();
    rivalSkillPoint = 0;
    
    skillUseId = -1;
    skillId = -1;
    targetPoint = Point(-1,-1);
    
    skillRivaUselId = -1;
    skillRivalId = -1;
    targetRivalPoint = Point(-1,-1);
    
    survive.clear();
    manhattanDistance = 0;


    preNinjaX0 = -1;
    preNinjaY0 = -1;
    preNinjaX1 = -1;
    preNinjaY1 = -1;
    manhattanPreDistance = 0;
    skillNumOfUse = 0;
    //
    killDog = 0;
    minSoulManhattanDistance.clear();
    ninjaConfined = false;

    //attack
    kill = -2;
    attackMode = false;
    cornerClosed = INF;
    surroundNumOfDog = 0;
  }

  static State input(int numOfSkills) {
    State st;

    cin >> st.skillPoint;
    cin >> st.H >> st.W;
    //togasaki
    //init global

    
    
    st.field.clear();

    for (int i = 0; i < st.H; i++) {
      vector<Cell> line;
      string s;
      cin >> s;
      for (int j = 0; j < s.size(); j++) {
        line.emplace_back(Cell(j, i, s[j]));
      }
      st.field.emplace_back(line);
    }

    int numOfNinjas;
    cin >> numOfNinjas;
    st.ninjas.clear();
    st.minDistSoulById.clear();



    
    for (int i = 0; i < numOfNinjas; i++) {
      Character ninja = Character::input();
      st.ninjas.emplace_back(ninja);
      if (i == 0){
	st.preNinjaX0 = ninja.x;
	st.preNinjaY0 = ninja.y;
      }else{
	st.preNinjaX1 = ninja.x;
	st.preNinjaY1 = ninja.y;
      }
      st.minDistSoulById.emplace_back(INF);
      st.minSoulManhattanDistance.emplace_back(INF);
      st.field[ninja.y][ninja.x].containsNinja = true;
    }

    int numOfDogs;
    cin >> numOfDogs;
    st.dogs.clear();
    for (int i = 0; i < numOfDogs; i++) {
      Character dog = Character::input();
      st.dogs.emplace_back(dog);
      st.field[dog.y][dog.x].containsDog = true;
    }

    int numOfSouls;
    cin >> numOfSouls;
    st.souls = vector<Point>();
    for (int i = 0; i < numOfSouls; i++) {
      int x, y;
      cin >> y >> x;
      st.souls.emplace_back(Point(x, y));
      st.field[y][x].containsSoul = true;
    }

    ///    st.skillCount = vector<int>();
    for (int i = 0; i < numOfSkills; i++) {
      int count;
      cin >> count;
      //      st.skillCount.emplace_back(count);
    }

    return st;
  }

  bool operator < (const State &right) const {
    
    for (int i = 0; i < survive.size(); i++){
      if (survive[i] < right.survive[i]){
    	return true;
      }
      if (survive[i] > right.survive[i]){
    	return false;
      }
      //kill
      if (kill < right.kill){
	return true;
      }
      if (kill > right.kill){
	return false;
      }
    }
    
    //閉じ込められてる
    if (ninjaConfined && !right.ninjaConfined){
      return true;
    }
    //閉じ込められてない
    if (!ninjaConfined && right.ninjaConfined){
      return false;
    }

    //Update二回術を使って一個多く手に入れた魂は嬉しくない
    
    if (getSoul - right.getSoul >= 1){
      if (skillNumOfUse - right.skillNumOfUse > 1){
	return true;
      }
      if (skillNumOfUse -right.skillNumOfUse <= 1){
	return false;
      }
    }
    
    //二個以上は嬉しいよ
    if (getSoul - right.getSoul >= 2){
      return false;
    }

    if (getSoul < right.getSoul){
      return true;
    }
    if (getSoul > right.getSoul){
      return false;
    }

    //忍者のマンハッタン距離
    if (manhattanDistance <= 10 && right.manhattanDistance > 10){
      return true;
    }
    if (manhattanDistance > 10 && right.manhattanDistance <= 10){
      return false;
    }
    

    //どん詰まり
    if (minDistSoulById[0] + minDistSoulById[1] == 2 * INF && right.minDistSoulById[0] + right.minDistSoulById[1] != 2 * INF){
      return true;
    }
    if (minDistSoulById[0] + minDistSoulById[1] != 2 * INF && right.minDistSoulById[0] + right.minDistSoulById[1] == 2 * INF){
      return false;
    }

    if (skillPoint < right.skillPoint){
      return true;
    }

    if (skillPoint > right.skillPoint){
      return false;
    }



    

    if (minDistSoulById[0] + minDistSoulById[1] ==  right.minDistSoulById[0] + right.minDistSoulById[1]){
      if (minSoulManhattanDistance[0] + minSoulManhattanDistance[1] > right.minSoulManhattanDistance[0] + right.minSoulManhattanDistance[1]){
	return true;
      }
      if (minSoulManhattanDistance[0] + minSoulManhattanDistance[1]< right.minSoulManhattanDistance[0] + right.minSoulManhattanDistance[1]){
	return false;
      }
      //cornerに近いかどうか
      if (cornerClosed < right.cornerClosed){
	return true;
      }
      if (cornerClosed > right.cornerClosed){
	return false;
      }
      //忍者のマンハッタン距離
      if (manhattanDistance < right.manhattanDistance){
	return true;
      }
      if (manhattanDistance > right.manhattanDistance){
	return false;
      }
      //前との距離
      if (manhattanPreDistance < right.manhattanPreDistance){
	return true;
      }
      if (manhattanPreDistance > right.manhattanPreDistance){
	return false;
      }
      //
      if (commandId < right.commandId){
	return true;
      }
      if (commandId > right.commandId){
	return false;
      }
    }
    return minDistSoulById[0] + minDistSoulById[1] > right.minDistSoulById[0] + right.minDistSoulById[1];
  }
};

class Search : public Point {
public:
  int dist;
  Search(){}
  Search(int x, int y, int dist): Point(x, y), dist(dist) {}
};

class Attack{
public:
  int skillId;
  Point targetPoint;
  Attack(){
    skillId = -1;
    targetPoint = Point(-1, -1);
  }
  Attack(int id, int x, int y):skillId(id),targetPoint(x, y){}
  Attack(int id, Point targetPoint):skillId(id),targetPoint(targetPoint){}
  void setSkill(int id, Point target){
    skillId = id;
    targetPoint = target;
  }
  
};

class Order{
public:
  int comId;
  int skillUseId;
  int skillId;
  int skillCost;
  Point targetPoint;
  Order(){
    comId = -1;
    skillId = -1;
    targetPoint = Point(-1, -1);
    skillCost = 0;
    skillUseId = -1;
  }
  Order(int comId, int skillUseId,int skillId, int skillCost,int x, int y):comId(comId),skillUseId(skillUseId),skillId(skillId),skillCost(skillCost),targetPoint(x, y){}

  void setOrder(int id){
    comId = id;
  }
  void setSkill(int useId, int id){
    skillUseId = useId;
    skillId = id;
    skillCost = skills[id].cost;
  }
  void setSkill(int id){
    skillId = id;
    skillCost = skills[id].cost;
  }
  void setTargetPoint(int x, int y){
    targetPoint = Point(x, y);
  }
};

class myQueue{
public:
  int head;
  int tail;
  Search queue[300];
  myQueue(){
    head = 0;
    tail = 0;
  }
  bool empty(){
    if (head == tail){
      return true;
    }else{
      return false;
    }
  }
  int size(){
    return (tail - head);
  }
  void push(const Search &p){
    queue[tail++] = p;
  }
  Search& front(){
    return queue[head];
  }
  void pop(){
    head++;
  }
  void init(){
    head = 0;
    tail = 0;
  }
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int remTime;
State myState;
State rivalState;
vector<int> commands;
//global
vector<vector<int> > DIST;
vector<vector<int> > CLOSED;
vector<int> firstDist,secondDist;
vector<Point> firstPoint,secondPoint;

vector<vector<Cell> > FIELD;
myQueue open;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void initBoard(vector<vector<int> > &array, int val){
  for (int y = 0; y < array.size(); y++){
    for (int x = 0; x < array[0].size(); x++){
      array[y][x] = val;
    }
  }

}

void useShadowCloneCornerPoint(const State &nowState, const Order &order, vector<Order> &result){
  if (nowState.skillPoint < skills[5].cost){
    return ;
  }
  Order next = order;
  int H = nowState.H;
  int W = nowState.W;
  for (int id = 0; id < 2; id++){
    int px = nowState.ninjas[id].x;
    int py = nowState.ninjas[id].y;
    //upper left;
    for (int y = H - 2; y >= 1; y--){
      for (int x = W - 2; x >= 1; x--){
	if (nowState.field[y][x].isWall() || nowState.field[y][x].isObject())continue;
	next.setSkill(5);
	next.setTargetPoint(x, y);
	result.push_back(next);
	goto NextSegment1;
      }
    }
  NextSegment1:;
    //upper right
    for (int y = H - 2; y >= 1; y--){
      for (int x = 1; x < nowState.W - 1; x++){
	if (nowState.field[y][x].isWall() || nowState.field[y][x].isObject())continue;
	next.setSkill(5);
	next.setTargetPoint(x, y);
	result.push_back(next);
	goto NextSegment2;
      }
    }
  NextSegment2:;

    //lower left
    for (int y = 1; y < nowState.H - 1; y++){
      for (int x = W - 1; x >= 1; x--){
	if (nowState.field[y][x].isWall() || nowState.field[y][x].isObject())continue;
	next.setSkill(5);
	next.setTargetPoint(x, y);
	result.push_back(next);
	goto NextSegment3;
      }
    }
  NextSegment3:;
    //lower right
    for (int y = 1; y < nowState.H - 1; y++){
      for (int x = 1; x < nowState.W - 1; x++){
	if (nowState.field[y][x].isWall() || nowState.field[y][x].isObject())continue;
	next.setSkill(5);
	next.setTargetPoint(x, y);
	result.push_back(next);
	goto NextSegment4;
      }
    }
  NextSegment4:;
  }
  return ;
}
void copyField(vector<vector<Cell> > &field, const vector<vector<Cell> > &copySource){
  
  for (int i = 0; i < field.size(); i++){
    for (int j = 0; j < field[0].size(); j++){
      field[i][j] = copySource[i][j];
    }
  }
  return ;
}


void useShadowCloneFarthestPoint(const State &nowState, const Order &order, vector<Order> &result){
  if (nowState.skillPoint < skills[5].cost){
    return ;
  }

  //initboard
  initBoard(DIST, INF);
  //  vector<vector<int> > dist(nowState.H, vector<int>(nowState.W, INF));
  Order next = order;
  int res = 0;
  Point targetPoint(-1, -1);
  for (int id = 0; id < 2; id++){
    int sx = nowState.ninjas[id].x;
    int sy = nowState.ninjas[id].y;
    //    queue<Search> open;
    open.init();
    //initboard
    initBoard(CLOSED, false);
    //    vector< vector<bool> > CLOSED(nowState.H, vector<bool>(nowState.W, false));
    //    vector< vector<Cell> > field = nowState.field;
    //copy
    copyField(FIELD, nowState.field);
    CLOSED[sy][sx] = true;
    DIST[sy][sx] = 0;	
    open.push(Search(sx, sy, 0));
    while (!open.empty()){
      Search sc = open.front();
      open.pop();
      if (!FIELD[sy][sx].isEmpty())continue;
      for (int dir = 0; dir < 4; dir++){
	int nx = sc.x + dx[dir];
	int ny = sc.y + dy[dir];
	if (FIELD[ny][nx].isWall())continue;
	if (CLOSED[ny][nx])continue;
	CLOSED[ny][nx] = true;
	if (DIST[ny][nx] > sc.dist + 1){
	  DIST[ny][nx] = sc.dist + 1;
	  open.push(Search(nx, ny, sc.dist + 1));
	}
	if (id == 1 && res <= DIST[ny][nx]){
	  if (res == DIST[ny][nx]){
	    int resX = targetPoint.x;
	    int resY = targetPoint.y;
	    if (resY == ny){
	      if (nx < resX){
		res = DIST[ny][nx];
		targetPoint = Point(nx, ny);
	      }
	    }else if (ny < resY){
	      res = DIST[ny][nx];
	      targetPoint = Point(nx, ny);
	    }
	  }else{
	    res = DIST[ny][nx];
	    targetPoint = Point(nx, ny);
	  }
	}
      }
    }
  }
  next.setSkill(5);
  next.setTargetPoint(targetPoint.x, targetPoint.y);
  result.emplace_back(next);
  return ;
}

void useShadowClone(const State& nowState, const Order &order, vector<Order> &result){
  if (nowState.skillPoint < skills[5].cost){
    return ;
  }
  
  Order next = order;
  for (int id = 0; id < 2; id++){
    int px = nowState.ninjas[id].x;
    int py = nowState.ninjas[id].y;
    for (int y = -1; y <= 1; y++){
      for (int x = -1; x <= 1; x++){
  	int ny = y + py;
  	int nx = x + px;
  	if (!nowState.field[ny][nx].isEmpty())continue;
  	next.setSkill(5);
  	next.setTargetPoint(nx, ny);
  	result.emplace_back(next);
      }
    }
  }

  return ;
}
void useLightning(const State& nowState, const Order &order, vector<Order> &result){
  if (nowState.skillPoint < skills[3].cost){
    return ;
  }
  Order next = order;
  const int comBits = commands[order.comId];
  int upperBit = comBits / pow5[2];
  int lowerBit = comBits - upperBit * pow5[2];
  //  int comBit = (id == 0 ? (upperBit / pow5[j]) % pow5[1] : (lowerBit / pow5[j]) % pow5[1]);
  
  for (int id = 0; id < 2; id++){
    int px = nowState.ninjas[id].x;
    int py = nowState.ninjas[id].y;
    for (int j = 0; j < 2; j++){
      int comBit = ((id == 0) ? (upperBit / pow5[j]) % pow5[1] : (lowerBit / pow5[j]) % pow5[1]);
      int nx = px + dx[comBit];
      int ny = py + dy[comBit];
      if (nowState.field[ny][nx].isWall())break;
      next.setSkill(3);
      next.setTargetPoint(nx, ny);
      result.emplace_back(next);
      int nnx = px + 2 * dx[comBit];
      int nny = py + 2 * dy[comBit];
      if (nowState.field[nny][nnx].isWall())continue;
      next.setSkill(3);
      next.setTargetPoint(nnx, nny);
      result.emplace_back(next);
      px = nx;
      py = ny;
    }
  }
  return ;
}

void attackFallRock(const State& myState, const State& rivalState, vector<Attack> &result){
  if (rivalState.skillPoint < skills[2].cost){
    return ;
  }
  Attack attack;
  for (int id = 0; id < 2; id++){
    int px = myState.ninjas[id].x;
    int py = myState.ninjas[id].y;
    for (int y = -3; y <= 3; y++){
      for (int x = -3; x <= 3; x++){
	int fallx = px + x;
	int fally = py + y;
	if (abs(x) + abs(y) > 3)continue;
	if (fallx <= 0 || fallx >= myState.W - 1 || fally <= 0 || fally >= myState.H - 1){
	  continue;
	}
	if (!myState.field[fally][fallx].isEmpty() || myState.field[fally][fallx].containsDog || myState.field[fally][fallx].containsNinja || myState.field[fally][fallx].containsSoul){
	  continue;
	}
	attack.setSkill(2, Point(fallx, fally));
	result.emplace_back(attack);
      }
    }
  }
  return ;
}

void attackShadowClone(const State& myState, const State& rivalState, vector<Attack> &result){
  if (rivalState.skillPoint < skills[6].cost){
    return ;
  }
  Attack attack;  
  for (int id = 0; id < 2; id++){
    int px = myState.ninjas[id].x;
    int py = myState.ninjas[id].y;
    for (int y = -2; y <= 2; y++){
     for (int x = -2; x <= 2; x++){
	int nx = px + x;
	int ny = py + y;
	if (abs(x) + abs(y) >= 3)continue;
	if (nx <= 0 || nx >= myState.W - 1 || ny <= 0 || ny >= myState.H - 1){
	  continue;
	}
	if (!myState.field[ny][nx].isEmpty()){
	  continue;
	}
	attack.setSkill(6, Point(nx, ny));
	result.emplace_back(attack);
      }
    }
  }
  return ;
}

bool checkReachDeath(const State& myState, const State& rivalState){
  for (int id = 0; id < 2; id++){
    int px = myState.ninjas[id].x;
    int py = myState.ninjas[id].y;
    for (int y = -3; y <= 3; y++){
      for (int x = -3; x <= 3; x++){
	if (abs(x) + abs(y) > 3)continue;
	int nx = px + x;
	int ny = py + y;
	if (nx <= 0 || nx >= myState.W - 1 || ny <= 0 || ny >= myState.H - 1){
	  continue;
	}
	if (myState.field[ny][nx].containsDog){
	  return true;
	}
      }
    }
  }
  return false;

}


void possibleAttack(vector<Attack> &result, const State& myState, const State& rivalState){
  //  result.emplace_back(Attack());//None
  if (!checkReachDeath(myState, rivalState))return ;
  attackFallRock(myState, rivalState, result);
  attackShadowClone(myState, rivalState, result);
  return ;
}
bool validateOrder(const State& nowState, int comId, int skillId){


  const int comBits = commands[comId];
  int upperBit = comBits / pow5[2];
  int lowerBit = comBits - upperBit * pow5[2];

  
  for (int id = 0; id < 2; id++){
    int px = nowState.ninjas[id].x;
    int py = nowState.ninjas[id].y;
    int nx = px;
    int ny = py;
    for (int i = 0; i < 2; i++){
      int comBit = ((id == 0) ? (upperBit / pow5[i]) % pow5[1] : (lowerBit / pow5[i]) % pow5[1]);
      nx += dx[comBit];
      ny += dy[comBit];
      if (nowState.field[ny][nx].isWall()){
	return false;
      }
      if (nowState.field[ny][nx].isObject()){
      	int nnx = nx + dx[comBit];	
      	int nny = ny + dy[comBit];
      	if (!nowState.field[nny][nnx].isEmpty() || nowState.field[nny][nnx].containsDog || nowState.field[nny][nnx].containsNinja){
      	  if (skillId != 3 || skillId != 5){
      	    return false;
      	  }
      	}
      }
    }
     for (int k = 0; k < 5; k++){
       int pnx = nx + dx[k];
       int pny = ny + dy[k];
       if (nowState.field[pny][pnx].containsDog && skillId != 5){//unused shadowclone
     	return false;
       }
     }
  }
  return true;
}

void useWhirlslash(const State& nowState, int id, const Order &order, vector<Order> &result, bool special=false){
  if (nowState.skillPoint < skills[7].cost){
    return ;
  }
  int px = nowState.ninjas[id].x;
  int py = nowState.ninjas[id].y;
  Order nextOrder = order;
  int dog = 0;
  for (int y = -1; y <= 1; y++){
    for (int x = -1; x <= 1; x++){
      if (nowState.field[py + y][px + x].containsDog){
	dog++;
      }
    }
  }
  if (special || (skills[7].cost > 15 && dog >= 4) || (skills[7].cost >= 9 && skills[7].cost <= 15 && dog >= 3) || (skills[7].cost < 9 && dog >= 2)){
    nextOrder.setSkill(id, 7);
    result.emplace_back(nextOrder);
  }

  return ;
}

void possibleOrder(vector<Order> &result, const State& nowState, int depth, bool useSpecialSkill, bool flagMyAttack=false){
  
  Order nowOrder;
  //  cerr << depth << " " << useSpecialSkill << endl;
  if (!flagMyAttack){//myOrder
    for (int i = 0; i < commands.size(); i++){
      nowOrder.setOrder(i);
      if (validateOrder(nowState, i, -1)){
	result.emplace_back(nowOrder);
      }
      if (depth == 0 && nowState.attackMode == true){
       	continue;
      }
      if (depth < 1 || useSpecialSkill){
	if (!useSpecialSkill){
	  //2
	  //5
	  useShadowCloneFarthestPoint(nowState, nowOrder, result);
	  useShadowCloneCornerPoint(nowState, nowOrder, result);
	  useLightning(nowState, nowOrder, result);
	  useShadowClone(nowState, nowOrder, result);
	  if (skills[7].cost <= 20 || nowState.skillPoint >= skills[7].cost * 1.5){
	    for (int id = 0; id < 2; id++){
	      useWhirlslash(nowState, id, nowOrder,result);
	    }
	  }
	}
	//7
	if (useSpecialSkill){
	  //2
	  for (int id = 0; id < 2; id++){
	    useWhirlslash(nowState, id, nowOrder,result,useSpecialSkill);
	  }
	}
      }
    }

  }else{//rival order
    for (int i = 0; i < commands.size(); i++){
      nowOrder.setOrder(i);
      if (validateOrder(nowState, i, -1)){
	result.emplace_back(nowOrder);
      }
      useShadowClone(nowState, nowOrder, result);
      useLightning(nowState, nowOrder, result);
    }
  }
  return ;
}


vector<int> createCommands(){
  vector<int> result;
  vector<int> tmp;
  for (int i = 0; i < 5; i++){
    for (int j = 0; j < 5; j++){
      if (i == 4)break;
      int bit = i * pow5[0] + j * pow5[1];
      tmp.push_back(bit);
    }
  }

  for (int i = 0; i < tmp.size(); i++){
    for (int j = 0; j < tmp.size(); j++){
      result.push_back(tmp[i] * pow5[2] + tmp[j] * pow5[0]);
    }
  }

  return result;
}

void initMinDistSoulById(vector<int> &minDistSoulById, int val){
  for (int i = 0; i < minDistSoulById.size(); i++){
    minDistSoulById[i] = val;
  }
  return ;
}
// vector<int> firsetDist,firstPoint;
// vector<Point> firstPoint,secondPoint;
void calculateMinDistToSoul(State &nowState){
  //  initBoard(dist, INF);
  initMinDistSoulById(nowState.minDistSoulById, INF);//init
  
  initMinDistSoulById(firstDist, INF);
  initMinDistSoulById(secondDist, INF);
    
  
  for (int id = 0; id < 2; id++){
      
    int sx = nowState.ninjas[id].x;
    int sy = nowState.ninjas[id].y;
    //    queue<Search> open;
    open.init();
    //initBoard
    initBoard(CLOSED, false);
    //vector< vector<Cell> > field = nowState.field;
    copyField(FIELD, nowState.field);
    CLOSED[sy][sx] = true;
    open.push(Search(sx, sy, 0));
    while (!open.empty()){
      Search sc = open.front();
      open.pop();
      if (!FIELD[sy][sx].isEmpty())continue;
      for (int dir = 0; dir < 4; dir++){
	int nx = sc.x + dx[dir];
	int ny = sc.y + dy[dir];
	if (FIELD[ny][nx].isWall())continue;
	if (CLOSED[ny][nx])continue;
       	if (FIELD[ny][nx].containsDog)continue;
	if (FIELD[ny][nx].isObject()){
	  int nnx = nx + dx[dir];
	  int nny = ny + dy[dir];
	  if (!FIELD[nny][nnx].isEmpty())continue;
	  swap(FIELD[nny][nnx].kind, FIELD[ny][nx].kind);
	}
	if (FIELD[ny][nx].containsSoul){
	  if (firstDist[id] > sc.dist + 1){//first
	    //update second
	    secondDist[id] = firstDist[id];
	    secondPoint[id] = firstPoint[id];
	    
	    //update first
	    firstDist[id] = sc.dist + 1;
	    firstPoint[id] = Point(nx, ny);
	    
	  }else if(secondDist[id] > sc.dist + 1){//only update second
	    secondDist[id] = sc.dist + 1;
	    secondPoint[id] = Point(nx, ny);
	  }
	}
	if (firstDist[id] != INF && secondDist[id] != INF){
	  goto NextNinjaId;
	}
	CLOSED[ny][nx] = true;
	open.push(Search(nx, ny, sc.dist + 1));
      }
    }
  NextNinjaId:;
  }

  
  if (firstPoint[0] == firstPoint[1]){//equal
    if (firstDist[0] <= firstDist[1]){
      nowState.minDistSoulById[0] = firstDist[0];
      nowState.minDistSoulById[1] = secondDist[1];
    }
    if (firstDist[0] > firstDist[1]){
      nowState.minDistSoulById[0] = secondDist[0];
      nowState.minDistSoulById[1] = firstDist[1];
    }
  }else{//not equal
    nowState.minDistSoulById[0] = firstDist[0];
    nowState.minDistSoulById[1] = firstDist[1];
  }

  //calculateminDist
  int taboo = -1;
  for (int id = 0; id < 2; id++){
    int px = nowState.ninjas[id].x;
    int py = nowState.ninjas[id].y;
    for (int i = 0; i < nowState.souls.size(); i++){
      if (id == 1 && taboo == i)continue;
      int sx = nowState.souls[i].x;
      int sy = nowState.souls[i].y;
      if (nowState.minSoulManhattanDistance[id] > abs(px - sx) + abs(py - sy)) {
	nowState.minSoulManhattanDistance[id] = abs(px - sx) + abs(py - sy);
	if (id == 0){
	  taboo = id;
	}
      }
    }
  }
  return ;
}


void simulateNextDog(State &nowState, const Order &myOrder, const Attack& rivalAttack){
  //initBoard
  initBoard(DIST, INF);
  //  vector<vector<int> > dist(nowState.H, vector<int>(nowState.W, INF));
  vector<Point> targetNinjas;
    if (myOrder.skillId == 5){
      int x = myOrder.targetPoint.x;
      int y = myOrder.targetPoint.y;
      if (x != - 1 && y != -1 && nowState.field[y][x].isEmpty()){
	targetNinjas.emplace_back(myOrder.targetPoint);
      }
    }
    if (rivalAttack.skillId == 6){
      targetNinjas.emplace_back(rivalAttack.targetPoint);
    }
    if (targetNinjas.empty()){
      for (int id = 0; id < 2; id++){
	targetNinjas.emplace_back(Point(nowState.ninjas[id].x, nowState.ninjas[id].y));
      }
    }
    //    cerr << targetNinjas.size() << endl;
    for (int id = 0; id < targetNinjas.size(); id++){
      int sx = targetNinjas[id].x;
      int sy = targetNinjas[id].y;
      //      queue<Search> open;
      open.init();
      //initBoard
      initBoard(CLOSED, false);
      //      vector< vector<bool> > CLOSED(nowState.H, vector<bool>(nowState.W, false));
      CLOSED[sy][sx] = true;
      open.push(Search(sx, sy, 0));
      while (!open.empty()) {
	Search sc = open.front(); open.pop();
	DIST[sc.y][sc.x] = sc.dist;
	for (int dir = 0; dir < 4; dir++) {
	  int nx = sc.x + dx[dir];
	  int ny = sc.y + dy[dir];
	  if (!nowState.field[ny][nx].isEmpty()) continue;
	  if (CLOSED[ny][nx]) continue;

	  CLOSED[ny][nx] = true;
	  if (DIST[ny][nx] > sc.dist + 1){
	    open.push(Search(nx, ny, sc.dist + 1));
	  }
	}
      }
    }
    vector<pair<int, int> > orderDog;
    for (int i = 0; i < nowState.dogs.size(); i++){
      int px = nowState.dogs[i].x;
      int py = nowState.dogs[i].y;
      orderDog.emplace_back(make_pair(DIST[py][px], i));
    }
    sort(orderDog.begin(), orderDog.end());
    for (int i = 0; i < orderDog.size(); i++){
      int id = orderDog[i].second;
      int px = nowState.dogs[id].x;
      int py = nowState.dogs[id].y;
      int nowDist = DIST[py][px];
      //    if (nowDist == 0)continue;
      //      cerr << "nowDist = " << nowDist << endl;
      for (int k = 0; k < 4; k++){
	int nx = px + dogDx[k];
	int ny = py + dogDy[k];
	int nextDist = DIST[ny][nx];
	if (nowState.field[ny][nx].isEmpty() && !nowState.field[ny][nx].containsDog && (nowDist - nextDist) == 1){
	  nowState.field[py][px].containsDog = false;
	  nowState.field[ny][nx].containsDog = true;
	  nowState.dogs[id].x = nx;
	  nowState.dogs[id].y = ny;
	  break;
	}
      }
    }
    return ;
}



//-1 killed 0 fail 1 success 
int genNextState(State &nextState, int comId, bool shadow=false){


  const int comBits = commands[comId];
  int upperBit = comBits / pow5[2];
  int lowerBit = comBits - upperBit * pow5[2];
  
  for (int id = 0; id < 2; id++){
    
    for (int i = 0; i < 2; i++){
      int comBit = ((id == 0) ? (upperBit / pow5[i]) % pow5[1] : (lowerBit / pow5[i]) % pow5[1]);
      int px = nextState.ninjas[id].x;
      int py = nextState.ninjas[id].y;
      int nx = px + dx[comBit];
      int ny = py + dy[comBit];
      if (nextState.field[ny][nx].isWall()){
	return -1;
      }

      if (nextState.field[ny][nx].isObject()){//rock
	//next empty
	int nnx = nx + dx[comBit];
	int nny = ny + dy[comBit];
	if (nextState.field[nny][nnx].isWall() || !nextState.field[nny][nnx].isEmpty() || nextState.field[nny][nnx].containsDog || nextState.field[nny][nnx].containsNinja){
	  //	  cerr << nx << " " << ny << " " << nnx << " " << nny << endl;
	  continue;
	}

	swap(nextState.field[ny][nx].kind, nextState.field[nny][nnx].kind);
      }

      if (nextState.field[ny][nx].containsSoul){//find soul
	nextState.skillPoint += 2;
	nextState.getSoul += 1;
	nextState.field[ny][nx].containsSoul = false;
	nextState.souls.erase( find(nextState.souls.begin(), nextState.souls.end(), Point(nx, ny)) );
      }
      //next
      nextState.field[py][px].containsNinja = false;
      nextState.field[ny][nx].containsNinja = true;
      nextState.ninjas[id].x = nx;
      nextState.ninjas[id].y = ny;

    }
    //next killed by dog
  }
  
  if (!shadow){
    for (int id = 0; id < 2; id++){
      for (int i = 0; i < 5; i++){
	int nx = nextState.ninjas[id].x + dx[i];
	int ny = nextState.ninjas[id].y + dy[i];
	if (nextState.field[ny][nx].containsDog){
	  return -1;//killed
	}
      }
    }
  }
  
  return 1;
}


void simulateAttack(State &nowState, const Attack &attack){
  int skillId = attack.skillId;
  int tarX = attack.targetPoint.x;
  int tarY = attack.targetPoint.y;
  if (skillId == 2){//falllock
    nowState.field[tarY][tarX].kind = 'O';
  }
  
  return;
}

void simulateDefence(State& nowState, int skillUseId,int skillId, Point targetPoint){
  int tarX = targetPoint.x;
  int tarY = targetPoint.y;
  if (skillId == -1){
    return ;
  }
  if (skillId == 3){//useLightning
    if (nowState.field[tarY][tarX].isObject()){
      nowState.field[tarY][tarX].kind = '_';
      return ;
    }
  }
  if (skillId == 5){//shadowclone
    if (nowState.field[tarY][tarX].isObject()){//obeject
      nowState.targetPoint = Point(-1, -1);
    }
    return ;
  }
  if (skillId == 7){
    int px = nowState.ninjas[skillUseId].x;
    int py = nowState.ninjas[skillUseId].y;
    for (int y = -1; y <= 1; y++){
      for (int x = -1; x <= 1; x++){
	int nx = px + x;
	int ny = py + y;
	if (nowState.field[ny][nx].containsDog){
	  nowState.field[ny][nx].containsDog = false;
	  nowState.killDog++;
	  nowState.dogs.erase( find(nowState.dogs.begin(), nowState.dogs.end(), Point(nx, ny)) );
	}
      }
    }
    return ;
  }
  return ;
  
}

int calculateManhattanPreDistance(State& state){
  
  int px = state.ninjas[0].x;
  int py = state.ninjas[0].y;
  int ppx = state.preNinjaX0;
  int ppy = state.preNinjaY0;

  
  int pqx = state.preNinjaX1;
  int pqy = state.preNinjaY1;
  int qx = state.ninjas[1].x;
  int qy = state.ninjas[1].y;
  state.manhattanPreDistance = abs(px - ppx) + abs(py - ppy) + abs(qx - pqx) + abs(qy - pqy);
  return state.manhattanPreDistance;
}

int calculateManhattanDistance(State& state){
  
  int px = state.ninjas[0].x;
  int py = state.ninjas[0].y;

  int qx = state.ninjas[1].x;
  int qy = state.ninjas[1].y;
  state.manhattanDistance = abs(px - qx) + abs(py - qy);
  return state.manhattanDistance;
}

int calculateManhattanNinjasDistance(State& state){
  int px = state.ninjas[0].x;
  int py = state.ninjas[0].y;
  
  int qx = state.ninjas[1].x;
  int qy = state.ninjas[1].y;

  state.manhattanDistance = abs(px - qx) + abs(py - qy);
  return state.manhattanDistance;
}
void calculateSurroundNumOfDog(State &state){
  
  for (int id = 0; id < 2; id++){
    int px = state.ninjas[id].x;
    int py = state.ninjas[id].y;
    int dog = 0;
    for (int y = -1; y <= 1; y++){
      for (int x = -1; x <= 1; x++){
	int nx = px + x;
	int ny = py + y;
	if (state.field[ny][nx].containsDog){
	  dog++;
	}
      }
    }
    state.surroundNumOfDog = max(state.surroundNumOfDog, dog);
  }
    
  return ;
}


void showState(const vector<State> &currentState){
  cerr << "-----------------------" << endl;
  for (int i = 0; i < currentState.size(); i++){
    cout << currentState[i].getSoul << endl;
  }
  cerr << "-----------------------" << endl;
}

void showBoard(const vector<vector<Cell> >& board){
  //  cerr << "-----------------------------" << endl;
  int H = board.size();
  int W = board[0].size();
  for (int y = 0; y < H; y++){
    for (int x = 0; x < W; x++){
      if (board[y][x].containsDog){
	cerr << "D ";
	continue;
      }
      if (board[y][x].containsNinja){
	cerr << "N ";
	continue;
      }
      cerr << board[y][x].kind << " ";
    }
    cerr << endl;
  }

  return;
}


bool pruningAttack(const State& nowState, const Order& nowOrder, const Attack& nowAttack){
  if (nowAttack.skillId == -1){
    return false;
  }

  const int comBits = commands[nowOrder.comId];
  int upperBit = comBits / pow5[2];
  int lowerBit = comBits - upperBit * pow5[2];


  int targetX = nowAttack.targetPoint.x;
  int targetY = nowAttack.targetPoint.y;

  for (int id = 0; id < 2; id++){
    int px = nowState.ninjas[id].x;
    int py = nowState.ninjas[id].y;
    for (int j = 0; j < 2; j++){
      int comBit = ((id == 0) ? (upperBit / pow5[j]) % pow5[1] : (lowerBit / pow5[j]) % pow5[1]);  
      int nx = px + dx[comBit];
      int ny = py + dy[comBit];
      if (abs(nx - targetX) + abs(ny - targetY) <= 2){
	return false;
      }
      px = nx;
      py = ny;
    }

  }
  return true;
}




void checkConfined(State &nowState){

  for (int id = 0; id < 2; id++){
    int px = nowState.ninjas[id].x;
    int py = nowState.ninjas[id].y;
    bool confined = true;
    for (int i = 0; i < 4; i++){
      int nx = px + dx[i];
      int ny = py + dy[i];
      if (nowState.field[ny][nx].isEmpty() && !nowState.field[ny][nx].containsDog){
	confined = false;
	break;
      }
    }
    if (confined){
      nowState.ninjaConfined = true;
    }
  }
  return ;
}
void calculateNearCorner(State &nowState){
  for (int id = 0; id < 2; id++){
    int px = nowState.ninjas[id].x;
    int py = nowState.ninjas[id].y;
    int minDist = INF;
    for (int i = 0; i < 4; i++){
      int dist = abs(px - cornerX[i]) + abs(py - cornerY[i]);
      minDist = min(minDist, dist);
    }
    nowState.cornerClosed = min(nowState.cornerClosed, minDist);
  }

  return ;
}


void attackPhase(const State& myState, const State& rivalState, vector<State> &result){
  vector<Order> rivalOrders;

  possibleOrder(rivalOrders, rivalState, 0, false, true);
  
  vector<Attack> myAttacks;
  myAttacks.emplace_back(Attack());//None
  possibleAttack(myAttacks, rivalState, myState);
  //cerr << "Attack = " << myAttacks.size() << " Order" << rivalOrders.size() << endl;
  for (int i = 0; i < myAttacks.size(); i++){
    int kill = 1;
    int skillId = myAttacks[i].skillId;
    Point targetPoint = myAttacks[i].targetPoint;

    for (int j = 0; j < rivalOrders.size(); j++){

      int comRivalId = rivalOrders[j].comId;
      int skillRivalUseId = rivalOrders[j].skillUseId;
      int skillRivalId = rivalOrders[j].skillId;
      int skillRivalCost = rivalOrders[j].skillCost;
      Point targetRivalPoint = rivalOrders[j].targetPoint;
      //nextState

      State nextRivalState = rivalState;
      simulateAttack(nextRivalState, myAttacks[i]);
      simulateDefence(nextRivalState, skillRivalUseId, skillRivalId, targetRivalPoint);
      int tmp = genNextState(nextRivalState, comRivalId, skillRivalId == 5);
      if (tmp == -1){//killed
	// if (myAttacks[i].skillId != -1){//use skill

	// }else{//unused skill
	  
	// }
      }
      if (tmp == 1){//survive
	if (skillRivalId == 5 || myAttacks[i].skillId == 6){//use shadowClaone
	  simulateNextDog(nextRivalState, rivalOrders[j], myAttacks[i]);
	  bool flagDeath = false;
	  for (int id = 0; id < 2; id++){
	    int x = nextRivalState.ninjas[id].x;
	    int y = nextRivalState.ninjas[id].y;
	    if (nextRivalState.field[y][x].containsDog){//death
	      flagDeath = true;
	      break;
	      if (myAttacks[i].skillId != -1){//use skillId

	      }else{//unused skill

	      }
	    }
	  }
	  if (!flagDeath){//Survive!!!!!!!!!!!!!!!!!!!
	    kill = -2;
	    break;
	  }
	}else{
	  kill = -2;
	  break;
	}
      }
    }
    if (skillId == -1 || kill == 1){//Kill!!!!!!!!!!!!!!!
      State nextMyState = myState;

      nextMyState.kill = kill;
      nextMyState.skillId = skillId;
      nextMyState.targetPoint = targetPoint;
      //      cerr << skillId << " " << targetPoint.y << " " << targetPoint.x << endl;
      //assert(kill != 1);
      if (skillId != -1){//use attack
	nextMyState.attackMode = true;
	nextMyState.skillPoint -= skills[skillId].cost;
	//	cerr << "KILLLLLLL" << endl;
      }

      result.emplace_back(nextMyState);
    }
  }

  return ;
}
//比較関数
////////////////////////////////////////
bool PriorityWhirlslash(const State& left, const State& right){
    
  for (int i = 0; i < left.survive.size(); i++){
    if (left.survive[i] < right.survive[i]){
      return true;
    }
    if (left.survive[i] > right.survive[i]){
      return false;
    }
    //kill
    if (left.kill < right.kill){
      return true;
    }
    if (left.kill > right.kill){
      return false;
    }
  }
    
  //very low
  if (left.killDog < right.killDog){
    return true;
  }
  if (left.killDog > right.killDog){
    return false;
  }
  
  
  if (left.getSoul < right.getSoul){
    return true;
  }
  if (left.getSoul > right.getSoul){
    return false;
  }

  if (left.skillPoint >= skills[7].cost && right.skillPoint >= skills[7].cost){
    if (left.surroundNumOfDog < right.surroundNumOfDog){
      return true;
    }
    if (left.surroundNumOfDog > right.surroundNumOfDog){
      return false;
    }
  }

  
  //忍者のマンハッタン距離
  if (left.manhattanDistance <= 10 && right.manhattanDistance > 10){
    return true;
  }
  if (left.manhattanDistance > 10 && right.manhattanDistance <= 10){
    return false;
  }
    

  //どん詰まり
  if (left.minDistSoulById[0] + left.minDistSoulById[1] == 2 * INF && right.minDistSoulById[0] + right.minDistSoulById[1] != 2 * INF){
    return true;
  }
  if (left.minDistSoulById[0] + left.minDistSoulById[1] != 2 * INF && right.minDistSoulById[0] + right.minDistSoulById[1] == 2 * INF){
    return false;
  }

  if (left.skillPoint < right.skillPoint){
    return true;
  }

  if (left.skillPoint > right.skillPoint){
    return false;
  }
  if (left.minDistSoulById[0] + left.minDistSoulById[1] ==  right.minDistSoulById[0] + right.minDistSoulById[1]){
    if (left.minSoulManhattanDistance[0] + left.minSoulManhattanDistance[1] > right.minSoulManhattanDistance[0] + right.minSoulManhattanDistance[1]){
      return true;
    }
    if (left.minSoulManhattanDistance[0] + left.minSoulManhattanDistance[1]< right.minSoulManhattanDistance[0] + right.minSoulManhattanDistance[1]){
      return false;
    }
    //cornerに近いかどうか
    if (left.cornerClosed < right.cornerClosed){
      return true;
    }
    if (left.cornerClosed > right.cornerClosed){
      return false;
    }
    //忍者のマンハッタン距離
    if (left.manhattanDistance < right.manhattanDistance){
      return true;
    }
    if (left.manhattanDistance > right.manhattanDistance){
      return false;
    }
    //前との距離
    if (left.manhattanPreDistance < right.manhattanPreDistance){
      return true;
    }
    if (left.manhattanPreDistance > right.manhattanPreDistance){
      return false;
    }
    if (left.commandId < right.commandId){
      return true;
    }
    if (left.commandId > right.commandId){
      return false;
    }
  }
  return left.minDistSoulById[0] + left.minDistSoulById[1] > right.minDistSoulById[0] + right.minDistSoulById[1];
}

////////////////////////////////////////
void sortState(vector<State> &states){
  
  if ((skills[7].cost <= 10 || myState.skillPoint >= skills[7].cost * 1.5)){//
    sort(states.rbegin(), states.rend(), PriorityWhirlslash);    
    return ;
  }
  
  sort(states.rbegin(), states.rend());//ordinary
  return ;
}

void nthState(vector<State> &states, int beamWidth){
  
  if ((skills[7].cost <= 10 || myState.skillPoint >= skills[7].cost * 1.5)){//
    nth_element(states.rbegin(), states.rbegin() + beamWidth, states.rend(), PriorityWhirlslash);    
    return ;
  }
  nth_element(states.rbegin(), states.rbegin() + beamWidth, states.rend());
  //  cerr << "unko" << endl;
  return ;
}

/*
 * このAIについて
 * - 各忍者について、 thinkByNinja(id) を2回行います。
 * - thinkByNinja(id) は、一人の忍者の一歩の行動を決める関数です(詳しくは関数のコメントを参照)。
 *
 * - 忍術
 * -- 「超高速」のみを使用します。
 * -- 「超高速」を使えるだけの忍力を所持している場合に自動的に使用して、thinkByNinja(id) を1回多く呼び出します。
 */
void think(int depthLimit, int beamWidth=1000) {

  if (remTime <= 10000){//panic mode
    depthLimit = 2;
    beamWidth = 100;
  }
  vector<State> currentState[depthLimit + 1];
  attackPhase(myState, rivalState, currentState[0]);

  //depth 0
  int cntChallenge = 0;
  for (int depth = 0; depth < depthLimit; depth++){

    if (currentState[depth].size() > beamWidth){
      sortState(currentState[depth]);
      //nthState(currentState[depth], beamWidth);
      currentState[depth].resize(beamWidth);
    }
    cntChallenge++;
    bool flagSurvive = false;
    for (int k = 0; k < currentState[depth].size(); k++){
      vector<Order> myOrders;
      possibleOrder(myOrders,currentState[depth][k], depth, cntChallenge >= 2);
      vector<Attack> rivalAttacks;
      rivalAttacks.emplace_back(Attack());//none
      if (depth == 0){
	possibleAttack(rivalAttacks, currentState[depth][k], rivalState);
      }
      //cerr <<currentState[depth].size() << " " << myOrders.size() << " " << rivalAttacks.size() << endl;      
      for (int i = 0; i < myOrders.size(); i++){
	int survive = 1;
	int comId = myOrders[i].comId;
	int skillUseId = myOrders[i].skillUseId;
	int skillId = myOrders[i].skillId;
	int skillCost = myOrders[i].skillCost;
	Point targetPoint = myOrders[i].targetPoint;

	//rival
	Point targetRivalPoint = Point(-1, -1);
	int skillRivalId = -1;
	int skillRivalCost = skillRivalId >= 0 ? skills[skillRivalId].cost : 0;
	for (int j = 0; j < rivalAttacks.size(); j++){
	  State nextState = currentState[depth][k];
	  if (pruningAttack(nextState, myOrders[i], rivalAttacks[j])){
	     continue;
	  }
	  
	  simulateAttack(nextState, rivalAttacks[j]);
	  simulateDefence(nextState, skillUseId, skillId, targetPoint);
	  int tmp = genNextState(nextState, comId, skillId == 5);

	  if (tmp == -1){//killed
	    if (rivalAttacks[j].skillId != -1){//use skill
	      survive = -1;
	      skillRivalId = rivalAttacks[j].skillId;
	      targetRivalPoint = rivalAttacks[j].targetPoint;
	    }else{//unused skill
	      survive = -2;
	      break;
	    }
	  }

	  if (tmp == 1){//survive
	    if (skillId == 5 || rivalAttacks[j].skillId == 6){//use shadowClaone
	      simulateNextDog(nextState, myOrders[i], rivalAttacks[j]);
	      for (int id = 0; id < 2; id++){
	    	int x = nextState.ninjas[id].x;
	    	int y = nextState.ninjas[id].y;
		if (nextState.field[y][x].containsDog){//death
	    	  if (rivalAttacks[j].skillId != -1){//use skillId
	    	    survive = -1;
		    skillRivalId = rivalAttacks[j].skillId;
		    targetRivalPoint = rivalAttacks[j].targetPoint;
	    	  }else{//unused skill
	    	    survive = -2;
	    	    break;
	    	  }
	    	}
	      }
	    }
	  }
	  if (survive == -2)break;
	}
	if (survive != -2){
	  if (survive == 1)flagSurvive = true;
	  int comBits = commands[comId];
	  
	  int upperBit = comBits / pow5[2];
	  int lowerBit = comBits - upperBit * pow5[2];
	  State nextState = currentState[depth][k];
	  Attack nowAttack = Attack(skillRivalId, targetRivalPoint);
	  //	  cerr << skillId << " " << targetPoint.y << " " << targetPoint.x << endl;
	  //assert(survive != 1);
	  //ignore
	  //simulateAttack(nextState, nowAttack);//defence
	  //nextState.rivalSkillPoint -= skillRivalCost;
	  simulateDefence(nextState, skillUseId, skillId, targetPoint);//defence
	  genNextState(nextState, comId, skillId == 5);//survive
	  simulateNextDog(nextState, myOrders[i], nowAttack);//attack
	  
	  nextState.survive.emplace_back(survive);
	  if (depth == 0){//use skill
	    nextState.commandId = comId;
	    if (skillId != -1){
	      nextState.skillId = skillId;
	      nextState.skillUseId = skillUseId;
	      nextState.targetPoint = targetPoint;
	    }
	  }
	  if (skillId != -1){//useSkill
	    nextState.skillNumOfUse += 1;
	  }
	  nextState.skillPoint -= skillCost;
	  
	  calculateMinDistToSoul(nextState);
	  calculateManhattanPreDistance(nextState);
	  checkConfined(nextState);
	  calculateNearCorner(nextState);
	  calculateSurroundNumOfDog(nextState);
	  int manhattanDistance = calculateManhattanDistance(nextState);
	  currentState[depth + 1].emplace_back(nextState);
	}
      }
    }
    
    //    currentState[depth].clear();
    if (cntChallenge == 1){
      //      nthState(currentState[depth + 1], 1);
      //      sort(currentState[depth + 1].rbegin(), currentState[depth + 1].rend());
      //      cerr << maxState.survive.size() << " " << maxState.survive[maxState.survive.size() - 1] << endl;
      //      cerr << "depth = " << depth << endl;
      if (currentState[depth + 1].empty() || !flagSurvive){//use special skill
	//      if (currentState[depth + 1].empty() || currentState[depth + 1][0].survive[depth] != 1){//use special skill

	//	cerr << "PROBABLY DEATH!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
	//	currentState[depth + 1].clear();
	depth -= 1;
	continue;
      }
      //cerr << "unko" << endl;
    }
    cntChallenge = 0;
  }

  for (int depth = depthLimit; depth >= 1; depth--){
    sortState(currentState[depth]);
    //    sort(currentState[depth].rbegin(), currentState[depth].rend());
    cerr << currentState[depth].size() << endl;
    for (int i = 0; i < currentState[depth].size(); i++){
      int comId = currentState[depth][i].commandId;
      int skillUseId = currentState[depth][i].skillUseId;
      int skillId = currentState[depth][i].skillId;
      int tarX = currentState[depth][i].targetPoint.x;
      int tarY = currentState[depth][i].targetPoint.y;
      int survive = currentState[depth][i].survive[0];

      if (depth != 1 && survive != 1)continue;//not alive
      int p1x = currentState[depth][i].ninjas[0].x;
      int p1y = currentState[depth][i].ninjas[0].y;
      int p2x = currentState[depth][i].ninjas[1].x;
      int p2y = currentState[depth][i].ninjas[1].y;
      //      cerr << "survive = " << survive << endl;

      cerr << "skillId = " << skillId << endl;
      cerr << "comId = " << comId << " " << currentState[depth][i].survive[0] << " " << currentState[depth][i].kill << endl;


      if (skillId != -1){//use skill
	cout << 3 << endl;
	if (skillId == 0){
	  cout << skillId << endl;
	}
	if (skillId != 7 && skillId != 0){//not boost and kaitengiri

	  cout << skillId << " " << tarY << " " << tarX << endl;
	}
	
	if (skillId == 7){
	  cout << skillId << " " << skillUseId << endl;
	}

	const int comBits = commands[comId];
	int upperBit = comBits / pow5[2];
	int lowerBit = comBits - upperBit * pow5[2];

	for (int id = 0; id < 2; id++){
	  for (int j = 0; j < 2; j++){
	    int comBit = ((id == 0) ? (upperBit / pow5[j]) % pow5[1] : (lowerBit / pow5[j]) % pow5[1]);
	    cout << ds[comBit];
	  }
	  cout << endl;
	}
	return ;
      }else{//unused skill
	const int comBits = commands[comId];
	int upperBit = comBits / pow5[2];
	int lowerBit = comBits - upperBit * pow5[2];

	cout << 2 << endl;
	for (int id = 0; id < 2; id++){
	  for (int j = 0; j < 2; j++){
	    int comBit = ((id == 0) ? (upperBit / pow5[j]) % pow5[1] : (lowerBit / pow5[j]) % pow5[1]);
	    cout << ds[comBit];
	  }
	  cout << endl;
	}
	return ;
      }
    }
  }
  cout << 2 << endl;
  cout << endl;
  cout << endl;
  return ;
}

bool input() {
  if (!(cin >> remTime)) return false;

  int numOfSkills;
  cin >> numOfSkills;

  skills.clear();
  for (int i = 0; i < numOfSkills; i++) {
    skills.emplace_back(Skill::input(i));
  }

  myState = State::input(skills.size());
  rivalState = State::input(skills.size());
  myState.rivalSkillPoint = rivalState.skillPoint;
  return true;
}

void initGlobal(){
  CLOSED.resize(HEIGHT, vector<int>(WIDTH, 0));
  DIST.resize(HEIGHT, vector<int>(WIDTH, 0));
  FIELD.resize(HEIGHT, vector<Cell>(WIDTH));
    //global
  firstDist.resize(2, INF);
  secondDist.resize(2, INF);
  firstPoint.resize(2);
  secondPoint.resize(2);
  return ;
}


int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(0);
  // AIの名前を出力
  cout << "TogaTogAI" << endl;
  cout.flush();
  commands = createCommands();
  initGlobal();
  int turn = 1;
  while (input()) {
    think(3);
    cout.flush();

  }

  return 0;
}
