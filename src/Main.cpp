#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <set>
using namespace std;

const int INF = 876765346;

const char CELL_EMPTY = '_';
const char CELL_WALL ='W';
const char CELL_OBJECT = 'O';

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
  vector<int> skillCount;
  //togasaki
  double searchValue;
  int getSoul;
  bool fail;
  int commandId;
  vector<int> minDistSoulById;
  int rivalSkillPoint;

  int skillId;
  int skillDepth;
  Point targetPoint;
  int nextRivalAttack;
  
  int skillRivalId;
  int skillRivalDepth;
  Point targetRivalPoint;
  State() {
    skillPoint = H = W = -1;
    field.clear();
    ninjas.clear();
    dogs.clear();
    souls.clear();
    skillCount.clear();
    //togasaki
    searchValue = 0;
    getSoul = 0;
    fail = false;
    commandId = -1;
    minDistSoulById.clear();
    rivalSkillPoint = 0;
    skillId = -1;
    skillDepth = -1;
    targetPoint = Point(-1,-1);
    nextRivalAttack = INF;
    
    skillRivalId = -1;
    skillRivalDepth = -1;
    targetRivalPoint = Point(-1,-1);
  }

  static State input(int numOfSkills) {
    State st;

    cin >> st.skillPoint;
    cin >> st.H >> st.W;

    st.field.clear();

    for (int i = 0; i < st.H; i++) {
      vector<Cell> line;
      string s;
      cin >> s;
      for (int j = 0; j < s.size(); j++) {
        line.push_back(Cell(j, i, s[j]));
      }
      st.field.push_back(line);
    }

    int numOfNinjas;
    cin >> numOfNinjas;
    st.ninjas.clear();
    st.minDistSoulById.clear();
    for (int i = 0; i < numOfNinjas; i++) {
      Character ninja = Character::input();
      st.ninjas.push_back(ninja);
      st.minDistSoulById.push_back(INF);
      st.field[ninja.y][ninja.x].containsNinja = true;
    }

    int numOfDogs;
    cin >> numOfDogs;
    st.dogs.clear();
    for (int i = 0; i < numOfDogs; i++) {
      Character dog = Character::input();
      st.dogs.push_back(dog);
      st.field[dog.y][dog.x].containsDog = true;
    }

    int numOfSouls;
    cin >> numOfSouls;
    st.souls = vector<Point>();
    for (int i = 0; i < numOfSouls; i++) {
      int x, y;
      cin >> y >> x;
      st.souls.push_back(Point(x, y));
      st.field[y][x].containsSoul = true;
    }

    st.skillCount = vector<int>();
    for (int i = 0; i < numOfSkills; i++) {
      int count;
      cin >> count;
      st.skillCount.push_back(count);
    }

    return st;
  }

  bool operator < (const State &right) const {
    // if (nextRivalAttack > right.nextRivalAttack){
    //   return true;
    // }
    // if (nextRivalAttack < right.nextRivalAttack){
    //   return false;
    // }
    if (getSoul == right.getSoul){
      if(minDistSoulById[0] + minDistSoulById[1] == right.minDistSoulById[0] + right.minDistSoulById[1]){
      	return skillPoint < right.skillPoint;
      }
      return minDistSoulById[0] + minDistSoulById[1] > right.minDistSoulById[0] + right.minDistSoulById[1];
    }
    return getSoul < right.getSoul;
  }
};

class Search : public Point {
public:
  int dist;
  Search(){}
  Search(int x, int y, int dist): Point(x, y), dist(dist) {}
};


int remTime;
vector<Skill> skills;
State myState;
State rivalState;

int dx[] =    {  0,   1,   0,  -1,   0};
int dy[] =    { -1,   0,   1,   0,   0};
string ds[] = {"U", "R", "D", "L",  ""};
//togasaki
int dogDx[] =    {  0,   -1,   1,  0,   0};//U L R D
int dogDy[] =    { -1,   0,   0,   1,   0};

vector<vector<string> > commands;

vector<vector<string> > createCommands(){
  vector<vector<string> > result;
  vector<string> tmp;
  for (int i = 0; i < 4; i++){
    for (int j = 0; j < 4; j++){
      string com = to_string(i) + to_string(j);
      tmp.push_back(com);
    }
  }

  for (int i = 0; i < tmp.size(); i++){
    for (int j = 0; j < tmp.size(); j++){
      vector<string> res;
      res.push_back(tmp[i]);
      res.push_back(tmp[j]);
      result.push_back(res);
    }
  }

  return result;
}

void calculateMinDistToSoul(State &nowState){

  for (int id = 0; id < 2; id++){
    int sx = nowState.ninjas[id].x;
    int sy = nowState.ninjas[id].y;
    queue<Search> open;
    vector< vector<bool> > closed(nowState.H, vector<bool>(nowState.W, false));

    vector< vector<Cell> > field = nowState.field;
    closed[sy][sx] = true;
    open.push(Search(sx, sy, 0));
    while (!open.empty()){
      Search sc = open.front();
      open.pop();

      if (!field[sy][sx].isEmpty())continue;
      for (int dir = 0; dir < 4; dir++){
	int nx = sc.x + dx[dir];
	int ny = sc.y + dy[dir];
	if (field[ny][nx].isWall())continue;
	if (closed[ny][nx])continue;

	if (field[ny][nx].isObject()){
	  int nnx = nx + dx[dir];
	  int nny = ny + dy[dir];
	  if (!field[nny][nnx].isEmpty())continue;
	  swap(field[nny][nnx].kind, field[ny][nx].kind);
	}
	if (field[ny][nx].containsSoul){
	  nowState.minDistSoulById[id] = sc.dist + 1;
	  goto NextId;
	}
	closed[ny][nx] = true;
	open.push(Search(nx, ny, sc.dist + 1));
      }
    }
  NextId:;
  }
  return ;

}


void simulateNextDog(State &nowState){

  vector<vector<int> > dist(nowState.H, vector<int>(nowState.W, INF));
  for (int id = 0; id < 2; id++){
    int sx = nowState.ninjas[id].x;
    int sy = nowState.ninjas[id].y;
    queue<Search> open;
    vector< vector<bool> > closed(nowState.H, vector<bool>(nowState.W, false));

    closed[sy][sx] = true;
    open.push(Search(sx, sy, 0));
    while (!open.empty()) {
      Search sc = open.front(); open.pop();
      dist[sc.y][sc.x] = sc.dist;

      for (int dir = 0; dir < 4; dir++) {
	int nx = sc.x + dx[dir];
	int ny = sc.y + dy[dir];

	if (!nowState.field[ny][nx].isEmpty()) continue;
	if (closed[ny][nx]) continue;

	closed[ny][nx] = true;
	if (dist[ny][nx] > sc.dist + 1){
	  open.push(Search(nx, ny, sc.dist + 1));
	}
      }
    }
  }
  vector<pair<int, int> > orderDog;
  for (int i = 0; i < nowState.dogs.size(); i++){
    int px = nowState.dogs[i].x;
    int py = nowState.dogs[i].y;
    orderDog.push_back(make_pair(dist[py][px], i));
  }
  sort(orderDog.begin(), orderDog.end());

  for (int i = 0; i < orderDog.size(); i++){
    int id = orderDog[i].second;
    int px = nowState.dogs[id].x;
    int py = nowState.dogs[id].y;
    int nowDist = dist[py][px];
    for (int k = 0; k < 4; k++){
      int nx = px + dogDx[k];
      int ny = py + dogDy[k];
      int nextDist = dist[ny][nx];
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


void simulateNextShadowDog(int y, int x,State &nowState){

  vector<vector<int> > dist(nowState.H, vector<int>(nowState.W, INF));

    int sx = x;
    int sy = y;
    queue<Search> open;
    vector< vector<bool> > closed(nowState.H, vector<bool>(nowState.W, false));
    closed[sy][sx] = true;
    open.push(Search(sx, sy, 0));
    while (!open.empty()) {
      Search sc = open.front(); open.pop();
      dist[sc.y][sc.x] = sc.dist;

      for (int dir = 0; dir < 4; dir++) {
	int nx = sc.x + dx[dir];
	int ny = sc.y + dy[dir];

	if (!nowState.field[ny][nx].isEmpty()) continue;
	if (closed[ny][nx]) continue;

	closed[ny][nx] = true;
	if (dist[ny][nx] > sc.dist + 1){
	  open.push(Search(nx, ny, sc.dist + 1));
	}
      }
    }

  vector<pair<int, int> > orderDog;
  for (int i = 0; i < nowState.dogs.size(); i++){
    int px = nowState.dogs[i].x;
    int py = nowState.dogs[i].y;
    orderDog.push_back(make_pair(dist[py][px], i));
  }
  sort(orderDog.begin(), orderDog.end());

  for (int i = 0; i < orderDog.size(); i++){
    int id = orderDog[i].second;
    int px = nowState.dogs[id].x;
    int py = nowState.dogs[id].y;
    int nowDist = dist[py][px];
    for (int k = 0; k < 4; k++){
      int nx = px + dogDx[k];
      int ny = py + dogDy[k];
      int nextDist = dist[ny][nx];
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


State genNextState(const State &nowState, const vector<string> &command, bool shadow=false){

  State nextState = nowState;
  bool flagFail = false;

  for (int id = 0; id < 2; id++){
    string com = command[id];

    for (int i = 0; i < com.size(); i++){
      int px = nextState.ninjas[id].x;
      int py = nextState.ninjas[id].y;

      int nx = px + dx[com[i] - '0'];
      int ny = py + dy[com[i] - '0'];
      //      cerr << px << " " << py << " " << nx << " " << ny << endl;
      //cerr << nextState.field.size() << " " << nextState.field[0].size() << endl;
      //cout << nextState.field[ny][nx].kind << " " << nextState.field[ny][nx].isWall() << endl;
      if (nextState.field[ny][nx].isWall()){
	flagFail = true;
	break;
      }
      //      cerr << px << " " << py << " " << nx << " " << ny << endl;
      if (nextState.field[ny][nx].isObject()){//rock
	//next empty
	int nnx = nx + dx[com[i] - '0'];
	int nny = ny + dy[com[i] - '0'];
	if (!(nnx >= 0 && nnx < nextState.W && nny >= 0 && nny < nextState.H) || !nextState.field[nny][nnx].isEmpty() || nextState.field[nny][nnx].containsDog || nextState.field[nny][nnx].containsNinja){
	  flagFail = true;
	  break;
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
    if (!shadow){
      for (int i = 0; i < 5; i++){
	int nx = nextState.ninjas[id].x + dx[i];
	int ny = nextState.ninjas[id].y + dy[i];
	if (nextState.field[ny][nx].containsDog){
	  flagFail = true;
	  break;
	}
      }
    }
    if (flagFail){
      break;
    }
  }

  if (flagFail){
    nextState.fail = true;
  }

  return nextState;
}





const int fallDx[] = {0, 1, 0, -1, 0, 2, 0, -2};
const int fallDy[] = {-1, 0, 1, 0, -2, 0, 2, 0};
bool vaildFallRockByRival(int id, int fallDir, const State &nowState){
  if (nowState.rivalSkillPoint < skills[2].cost){//unable fall rock
    return false;
  }
  int px = nowState.ninjas[id].x;
  int py = nowState.ninjas[id].y;
  int fallx = px + fallDx[fallDir];
  int fally = py + fallDy[fallDir];
  if (fallx <= 0 || fallx >= nowState.W - 1 || fally <= 0 || fally >= nowState.H - 1){//
    return false;
  }

  if (nowState.field[fally][fallx].isEmpty() && nowState.field[fally][fallx].isNoCharacter()){
    return true;
  }
  return false;
}
bool nextKilledByDogs(const State &nowState, const vector<string> &command){

  State nextState = nowState;
  for (int id = 0; id < 2; id++){
    string com = command[id];
    //    cerr << "com = " << com  << endl;
    for (int i = 0; i < com.size(); i++){
      int px = nextState.ninjas[id].x;
      int py = nextState.ninjas[id].y;

      int nx = px + dx[com[i] - '0'];
      int ny = py + dy[com[i] - '0'];
      if (nextState.field[ny][nx].isWall()){
	continue;
      }

      if (nextState.field[ny][nx].isObject()){//rock
	//next empty
	int nnx = nx + dx[com[i] - '0'];
	int nny = ny + dy[com[i] - '0'];
	if (!nextState.field[nny][nnx].isEmpty() || nextState.field[nny][nnx].containsDog || nextState.field[nny][nnx].containsNinja){
	  continue;
	}
	//	swap(nowState.field[ny][nx].kind, nowState.field[ny][nx].kind);
	swap(nextState.field[ny][nx].kind, nextState.field[nny][nnx].kind);
      }

      //next
      nextState.field[py][px].containsNinja = false;
      nextState.field[ny][nx].containsNinja = true;
      nextState.ninjas[id].x = nx;
      nextState.ninjas[id].y = ny;
    }

    //next killed by dog
    for (int i = 0; i < 5; i++){
      int nx = nextState.ninjas[id].x + dx[i];
      int ny = nextState.ninjas[id].y + dy[i];
      if (nextState.field[ny][nx].containsDog){
	return true;
      }
    }
  }


  for (int id = 0; id < 2; id++){
    for (int i = 0; i < 5; i++){
      int nx = nextState.ninjas[id].x + dx[i];
      int ny = nextState.ninjas[id].y + dy[i];
      //      cerr << nx << " " << ny << " " << nextState.field[ny][nx].containsDog << " " << nextState.field[ny][nx].containsNinja << endl;
      if (nextState.field[ny][nx].containsDog){
	return true;
      }
    }
  }

  return false;
}


Point vaildFallRockByMe(const State &nowState, int depthlimit=1){

  vector<State> currentState;
  set<Point> candidateFallRock;
  Point target;

  for (int id = 0; id < 2; id++){
    int px = nowState.ninjas[id].x;
    int py = nowState.ninjas[id].y;
    //    cerr << px <<" " << py << endl;
    for (int y = -2; y <= 2; y++){
      for (int x = -2; x <= 2; x++){
	int dist = abs(y) + abs(x);
	int nx = px + x;
	int ny = py + y;
	if (nx <= 0 || nx >= nowState.W - 1 || ny <= 0 || ny >= nowState.H - 1){
	  continue;
	}
	//cerr << ny << " " << nx << endl;
	if (nowState.field[ny][nx].isEmpty() && nowState.field[ny][nx].isNoCharacter()){
	  candidateFallRock.insert(Point(nx, ny));

	}
      }
    }
  }


  for (const auto &p : candidateFallRock){
    int tarx = p.x;
    int tary = p.y;
    //     cerr << p.x << " " << p.y << endl;
    State nextState = nowState;
    nextState.field[tary][tarx].kind = 'O';
    nextState.targetPoint = Point(tarx, tary);
    currentState.push_back(nextState);
    //    cerr << p.x << " " << p.y << endl;
  }

  for (int i = 0; i < currentState.size(); i++){
    State nextState = currentState[i];
    bool attack = true;
    int cnt = 0;
    //cerr << commands.size() << endl;
    //    cerr << "target = " << nextState.targetPoint.x << " " << nextState.targetPoint.y << endl;
    for (int j = 0; j < commands.size(); j++){
      if (nextKilledByDogs(nextState, commands[j])){
	cnt++;
	continue;
      }

      attack = false;

    }
    if ((commands.size() - cnt) <= 3 || attack){
      target.x = nextState.targetPoint.x;
      target.y = nextState.targetPoint.y;
      //cerr << "attack!!! " << target.x << " " << target.y << endl;
      return target;
    }
  }
  return target;
}


bool vaildLightningByMe(int id, int fallDir, const State &nowState){
  if (nowState.skillPoint < skills[3].cost){//unable fall rock
    return false;
  }
  int px = nowState.ninjas[id].x;
  int py = nowState.ninjas[id].y;
  int fallx = px + fallDx[fallDir];
  int fally = py + fallDy[fallDir];
  if (fallx <= 0 || fallx >= nowState.W - 1 || fally <= 0 || fally >= nowState.H - 1){//
    return false;
  }

  if (nowState.field[fally][fallx].isObject()){
    return true;
  }
  return false;
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
void think(int depthLimit, int beamWidth=50) {

  vector<State> currentState[depthLimit + 1];
  currentState[0].push_back(myState);
  for (int id = 0; id < 2; id++){
    int px = myState.ninjas[id].x;
    int py = myState.ninjas[id].y;
    bool surroundDog = false;
    for (int dir = 0; dir < 4; dir++){
      int nx = px + dx[dir];
      int ny = py + dy[dir];
      if (myState.field[ny][nx].containsDog){
	surroundDog = true;
	break;
      }
    }
    if (surroundDog){
      for (int dir = 0; dir < 7; dir++){
	int nx = px + fallDx[dir];
	int ny = py + fallDy[dir];
	if(vaildFallRockByRival(id, dir, myState)){
	  State nextState = myState;
	  nextState.field[ny][nx].kind = 'O';
	  nextState.rivalSkillPoint -= skills[2].cost;
	  nextState.skillRivalId = 2;
	  nextState.skillRivalDepth = 0;
	  nextState.targetRivalPoint = Point(nx, ny);
	  currentState[0].push_back(nextState);
	}
      }
    }
  }

  
  set<int> tabooCommands;
  // cerr << currentState[0].size() << endl;
  for (int depth = 0; depth < depthLimit; depth++){

    if (depth > 0 && currentState[depth].size() > beamWidth){
      sort(currentState[depth].rbegin(), currentState[depth].rend());
      currentState[depth].erase(currentState[depth].begin() + beamWidth, currentState[depth].end());
    }
    for (int i = 0; i < currentState[depth].size(); i++){
      State nowState = currentState[depth][i];
      int deathNodeCnt = 0;
      for (int j = 0; j < commands.size(); j++){
	State nextState = genNextState(nowState, commands[j]);
	if (nextState.fail){
	  if (depth == 0 && nextState.skillRivalDepth == 0){//attack and use skill
	    tabooCommands.insert(j);
	  }
	  deathNodeCnt++;
	  continue;
	}
	if (depth == 0){
	  nextState.commandId = j;
	}
	simulateNextDog(nextState);
	calculateMinDistToSoul(nextState);
	currentState[depth + 1].push_back(nextState);
      }

      if (commands.size() - deathNodeCnt == 0){//determine death
	//cerr << commands.size() - deathNodeCnt << endl;

	//use skill
	if (nowState.skillId != -1)continue;


	//light
	if (nowState.skillPoint >= skills[3].cost){
	  State nowSkillState = nowState;
	  for (int id = 0; id < 2; id++){
	    int px = nowSkillState.ninjas[id].x;
	    int py = nowSkillState.ninjas[id].y;
	    for (int dir = 0; dir < 7; dir++){
	      int nx = px + fallDx[dir];
	      int ny = py + fallDy[dir];
	      if(vaildLightningByMe(id, dir, nowSkillState)){
		State lightState = nowSkillState;
		lightState.field[ny][nx].kind = '_';

		lightState.skillPoint -= skills[3].cost;
		if (lightState.skillDepth == -1){
		  lightState.skillId = 3;
		  lightState.skillDepth = depth;
		}
		lightState.nextRivalAttack = commands.size() - deathNodeCnt;

		lightState.targetPoint = Point(nx, ny);
		//next
		for (int j = 0; j < commands.size(); j++){
		  State nextState = genNextState(lightState, commands[j]);
		  if (nextState.fail){
		    continue;
		  }
		  if (depth == 0){
		    nextState.commandId = j;
		  }
		  simulateNextDog(nextState);
		  calculateMinDistToSoul(nextState);
		  currentState[depth + 1].push_back(nextState);
		}
	      }
	    }
	  }
	}


	//Shadow
	if (nowState.skillPoint >= skills[5].cost){
	  for (int id = 0; id < 2; id++){
	    State nowSkillState = nowState;
	    int px = nowSkillState.ninjas[id].x;
	    int py = nowSkillState.ninjas[id].y;

	    for (int y = 1; y < nowSkillState.H - 1; y++){//upper left
	      for (int x = 1; x < nowSkillState.W - 1; x++){
		if (nowSkillState.field[y][x].isEmpty()){//place shadow (x,y)
		  int cnt = 0;
		  for (int j = 0; j < commands.size(); j++){
		    State nextState = genNextState(nowSkillState, commands[j], true);
		    if (nextState.fail){
		      continue;
		    }
		    if (depth == 0){
		      nextState.commandId = j;
		    }
		    //set skill
		    nextState.skillPoint -= skills[5].cost;
		    if (nextState.skillDepth == -1){
		      nextState.skillId = 5;
		      nextState.skillDepth = depth;
		    }
		    nextState.nextRivalAttack = commands.size() - deathNodeCnt;
		    nextState.targetPoint = Point(x, y);
		    simulateNextShadowDog(y, x,nextState);

		    bool death = false;
		    for (int tmp = 0; tmp < 2; tmp++){
		      int px = nextState.ninjas[tmp].x;
		      int py = nextState.ninjas[tmp].y;
		      if (nextState.field[py][px].containsDog){
			death = true;
			break;
		      }
		    }
		    if (death)continue;
		    calculateMinDistToSoul(nextState);
		    currentState[depth + 1].push_back(nextState);
		    cnt++;
		  }
		  if (cnt != 0){
		    goto NextSegment1;
		  }
		}
	      }
	    }
	  }
	  NextSegment1:;

	  for (int id = 0; id < 2; id++){
	    State nowSkillState = nowState;
	    int px = nowSkillState.ninjas[id].x;
	    int py = nowSkillState.ninjas[id].y;

	    for (int y = 1; y < nowSkillState.H - 1; y++){//upper right
	      for (int x = nowSkillState.W - 2; x >= 1; x--){
		if (nowSkillState.field[y][x].isEmpty()){//place shadow (x,y)
		  int cnt = 0;
		  for (int j = 0; j < commands.size(); j++){
		    State nextState = genNextState(nowSkillState, commands[j], true);
		    if (nextState.fail){
		      continue;
		    }
		    if (depth == 0){
		      nextState.commandId = j;
		    }
		    //set skill
		    nextState.skillPoint -= skills[5].cost;
		    if (nextState.skillDepth == -1){
		      nextState.skillId = 5;
		      nextState.skillDepth = depth;
		    }
		    nextState.nextRivalAttack = commands.size() - deathNodeCnt;
		    nextState.targetPoint = Point(x, y);
		    simulateNextShadowDog(y, x,nextState);

		    bool death = false;
		    for (int tmp = 0; tmp < 2; tmp++){
		      int px = nextState.ninjas[tmp].x;
		      int py = nextState.ninjas[tmp].y;
		      if (nextState.field[py][px].containsDog){
			death = true;
			break;
		      }
		    }
		    if (death)continue;
		    calculateMinDistToSoul(nextState);
		    currentState[depth + 1].push_back(nextState);
		    cnt++;
		  }
		  if (cnt != 0){
		    goto NextSegment2;
		  }
		}
	      }
	    }
    }
	  NextSegment2:;

	  for (int id = 0; id < 2; id++){
	    State nowSkillState = nowState;
	    int px = nowSkillState.ninjas[id].x;
	    int py = nowSkillState.ninjas[id].y;

	    for (int y = nowSkillState.H - 2; y >= 1; y--){//lower left
	      for (int x = 1; x < nowSkillState.W - 1; x++){
		if (nowSkillState.field[y][x].isEmpty()){//place shadow (x,y)
		  int cnt = 0;
		  for (int j = 0; j < commands.size(); j++){
		    State nextState = genNextState(nowSkillState, commands[j], true);
		    if (nextState.fail){
		      continue;
		    }
		    if (depth == 0){
		      nextState.commandId = j;
		    }
		    //set skill
		    nextState.skillPoint -= skills[5].cost;
		    if (nextState.skillDepth == -1){
		      nextState.skillId = 5;
		      nextState.skillDepth = depth;
		    }
		    nextState.nextRivalAttack = commands.size() - deathNodeCnt;
		    nextState.targetPoint = Point(x, y);
		    simulateNextShadowDog(y, x,nextState);

		    bool death = false;
		    for (int tmp = 0; tmp < 2; tmp++){
		      int px = nextState.ninjas[tmp].x;
		      int py = nextState.ninjas[tmp].y;
		      if (nextState.field[py][px].containsDog){
			death = true;
			break;
		      }
		    }
		    if (death)continue;
		    calculateMinDistToSoul(nextState);
		    currentState[depth + 1].push_back(nextState);
		    cnt++;
		  }
		  if (cnt != 0){
		    goto NextSegment3;
		  }
		}
	      }
	    }
    }
	  NextSegment3:;
	  for (int id = 0; id < 2; id++){
	    State nowSkillState = nowState;
	    int px = nowSkillState.ninjas[id].x;
	    int py = nowSkillState.ninjas[id].y;

	    for (int y = nowSkillState.H - 2; y >= 1; y--){//lower right
	      for (int x = nowSkillState.W - 2; x >= 1; x--){
		if (nowSkillState.field[y][x].isEmpty()){//place shadow (x,y)
		  int cnt = 0;
		  for (int j = 0; j < commands.size(); j++){
		    State nextState = genNextState(nowSkillState, commands[j], true);
		    if (nextState.fail){
		      continue;
		    }
		    if (depth == 0){
		      nextState.commandId = j;
		    }
		    //set skill
		    nextState.skillPoint -= skills[5].cost;
		    if (nextState.skillDepth == -1){
		      nextState.skillId = 5;
		      nextState.skillDepth = depth;
		    }

		    nextState.nextRivalAttack = commands.size() - deathNodeCnt;
		    nextState.targetPoint = Point(x, y);
		    simulateNextShadowDog(y, x,nextState);

		    bool death = false;
		    for (int tmp = 0; tmp < 2; tmp++){
		      int px = nextState.ninjas[tmp].x;
		      int py = nextState.ninjas[tmp].y;
		      if (nextState.field[py][px].containsDog){
			death = true;
			break;
		      }
		    }
		    if (death)continue;
		    calculateMinDistToSoul(nextState);
		    currentState[depth + 1].push_back(nextState);
		    cnt++;
		  }
		  if (cnt != 0){
		    goto NextSegment4;
		  }
		}
	      }
	    }
	  NextSegment4:;
	  }
	}

      }
    }

    if (depth == 0){
      //      cerr << tabooCommands.size() << endl;
      int last = currentState[depth + 1].size();
      for (int i = 0; i < last; i++){
      	//simulate
	int comId = currentState[depth + 1][i].commandId;
	int skillId = currentState[depth + 1][i].skillId;
	int skillDepth = currentState[depth + 1][i].skillDepth;
	int skillRivalId = currentState[depth + 1][i].skillRivalId;
	int skillRivalDepth = currentState[depth + 1][i].skillRivalDepth;	
	State originState = myState;
	if (skillDepth != 0 && tabooCommands.count(comId) > 0){//first not use skill and probable death
	   continue;
	 }
      	if (skillRivalDepth == 0){//rival use skill attack
	  if (skillDepth == 0){//first I use skill
	    int targetX = currentState[depth + 1][i].targetPoint.x;
	    int targetY = currentState[depth + 1][i].targetPoint.y;
	    if (skillId == 3){
	      originState.field[targetY][targetX].kind = '_';
	      State nextState = genNextState(originState, commands[comId]);
	      if (nextState.fail){
		continue;
	      }
	    }
	    if (skillId == 5){//shadow
	      State nextState = genNextState(originState, commands[comId], true);
	      if (nextState.fail){
		continue;
	      }
	      simulateNextShadowDog(targetY, targetX,nextState);
	      bool death = false;
	      for (int tmp = 0; tmp < 2; tmp++){
		int px = nextState.ninjas[tmp].x;
		int py = nextState.ninjas[tmp].y;
		if (nextState.field[py][px].containsDog){
		  death = true;
		  break;
		}
	      }
	      if (depth)continue;
	    }
	  }else{
	    
	    State nextState = genNextState(originState, commands[comId]);
	    if (nextState.fail)continue;
	  }
      	}
	//ok
	currentState[depth + 1].push_back(currentState[depth + 1][i]);
      }
      if (currentState[depth + 1].size() != last){
	currentState[depth + 1].erase(currentState[depth + 1].begin(), currentState[depth + 1].begin() + last);
      }
      //      cerr << currentState[depth + 1].size() << endl;
    }

  }
  cerr << "taboocommand.size() = " << tabooCommands.size() << endl;
  for (int depth = depthLimit; depth >= 1; depth--){
    sort(currentState[depth].rbegin(), currentState[depth].rend());
    if (currentState[depth].empty())continue;

    int comId = currentState[depth][0].commandId;
    int skillId = currentState[depth][0].skillId;
    int skillDepth = currentState[depth][0].skillDepth;
    int targetX = currentState[depth][0].targetPoint.x;
    int targetY = currentState[depth][0].targetPoint.y;

    cerr << currentState[depth][0].skillRivalId << " " << currentState[depth][0].targetRivalPoint.x << " " << currentState[depth][0].targetPoint.y << endl;
    if (skillDepth == 0){
      if (skillId == 2 && targetX != -1){
	cout << 3 << endl;
	cout << 2 << " " << targetY << " " << targetX << endl;
      }
      if (skillId == 3 && targetX != -1){
	cout << 3 << endl;
	cout << 3 << " " << targetY << " " << targetX << endl;
      }
      if (skillId == 5 && targetX != -1){
	cout << 3 << endl;
	cout << 5 << " " << targetY << " " << targetX << endl;
      }
    }else{
      cout << 2 << endl;
    }

    for (int i = 0; i < commands[comId].size(); i++){
      for (int j = 0; j < commands[comId][i].size(); j++){
	cout << ds[commands[comId][i][j] - '0'];
      }
      cout << endl;
    }
    return ;
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
    skills.push_back(Skill::input(i));
  }

  myState = State::input(skills.size());
  rivalState = State::input(skills.size());
  myState.rivalSkillPoint = rivalState.skillPoint;
  return true;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(0);
  // AIの名前を出力
  cout << "TogaTogAI" << endl;
  cout.flush();
  commands = createCommands();
  while (input()) {
    think(5);
    cout.flush();
  }

  return 0;
}
