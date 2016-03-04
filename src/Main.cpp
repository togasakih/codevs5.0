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
    if (getSoul == right.getSoul){
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

class MinDist {
public:
  int dogDist, soulDist;
  MinDist(int dogDist, int soulDist): dogDist(dogDist), soulDist(soulDist) {}
};

int remTime;
vector<Skill> skills;
State myState;
State rivalState;

int dx[] =    {  0,   1,   0,  -1,   0};
int dy[] =    { -1,   0,   1,   0,   0};
string ds[] = {"U", "R", "D", "L",  ""};
int dogDx[] =    {  0,   -1,   1,  0,   0};//U L R D
int dogDy[] =    { -1,   0,   0,   1,   0};
/**
 * (sx, sy) を起点として、幅優先探索で (最も近い忍犬までの距離, 最も近いアイテムまでの距離) を計算して MinDist で返します。
 * 忍犬やアイテムがフィールド上に存在しない場合は、対応する値を INF として返します。
 * (sx, sy) が歩けない場所である場合、 MinDist(-INF, -INF) として返します。
 */
MinDist getMinDist(const State &st, int sx, int sy) {
  if (!st.field[sy][sx].isEmpty()) {
    return MinDist(-INF, -INF);
  }

  MinDist minDist(INF, INF);
  queue<Search> open;
  vector< vector<bool> > closed(st.H, vector<bool>(st.W, false));
  closed[sy][sx] = true;
  open.push(Search(sx, sy, 0));

  while (!open.empty()) {
    Search sc = open.front(); open.pop();

    if (st.field[sc.y][sc.x].containsDog && minDist.dogDist == INF) {
      minDist.dogDist = sc.dist;
    }
    if (st.field[sc.y][sc.x].containsSoul && minDist.soulDist == INF) {
      minDist.soulDist = sc.dist;
    }

    if (minDist.dogDist != INF && minDist.soulDist != INF) return minDist;

    for (int dir = 0; dir < 4; dir++) {
      int nx = sc.x + dx[dir];
      int ny = sc.y + dy[dir];

      if (!st.field[ny][nx].isEmpty()) continue;
      if (closed[ny][nx]) continue;

      closed[ny][nx] = true;
      open.push(Search(nx, ny, sc.dist + 1));
    }
  }

  return minDist;
}

/*
 * 引数
 * - id: 忍者ID
 * - dir: 忍者を歩かせる方向
 *
 * ID が id である忍者を dir の方向へ移動するシミュレートを行います。
 * この関数で行われるシミュレート内容
 * - 忍者の位置修正 (移動先が岩の場合は、位置修正を行わずにシミュレートを終了します)
 * - 移動先にニンジャソウルが存在する場合、取得処理(忍力回復する, フィールドのソウルフラグをfalseにする, 取得済みのソウルの座標削除)が行われます。
 * (※簡単なシミュレートのため、壁を押すなどの処理は行われません)
 */
void simulateWalk(int id, int dir) {
  int nx = myState.ninjas[id].x + dx[dir];
  int ny = myState.ninjas[id].y + dy[dir];
  if (!myState.field[ny][nx].isEmpty()) return;

  myState.ninjas[id].x = nx;
  myState.ninjas[id].y = ny;

  if (!myState.field[ny][nx].containsSoul) return;

  // 忍力回復
  myState.skillPoint += 2;

  // フィールドのフラグをfalseに
  myState.field[ny][nx].containsSoul = false;

  // 取得済みのソウルの座標削除
  myState.souls.erase( find(myState.souls.begin(), myState.souls.end(), Point(nx, ny)) );
}


/*
 * 移動方向の決め方
 * - 忍者は、自分自身から最も近いニンジャソウルへ向かって移動します。
 * - 壁を押さずに移動します。
 * - 忍犬までの最短距離が1以下になるようなマスへは移動しません。
 * - 自分自身のマスから連結であるマスの中にニンジャソウルが存在しない場合は、忍犬までの最短距離が最大になるように移動します。
 * -- 忍犬も存在しない場合は、その場にとどまります。
 */
string thinkByNinjaId(int id) {
  vector<MinDist> dists;
  vector<Point> points;

  for (int dir = 0; dir < 5; dir++) {
    int nx = myState.ninjas[id].x + dx[dir];
    int ny = myState.ninjas[id].y + dy[dir];
    dists.push_back(getMinDist(myState, nx, ny));
    points.push_back(Point(nx, ny));
  }

  // ニンジャソウルへ近づく方向
  int minDistSoul = INF;
  int minDistSoulDir = 4;

  // 忍犬から遠ざかる方向
  int maxDistDog = -INF;
  int maxDistDogDir = 4;

  for (int dir = 0; dir < 5; dir++) {
    // この方向には歩くことができないので無視
    if (dists[dir].soulDist == -INF) continue;

    // 忍犬までの距離が1以下になってしまうので無視
    if (dists[dir].dogDist <= 1) continue;

    if (minDistSoul > dists[dir].soulDist) {
      minDistSoul = dists[dir].soulDist;
      minDistSoulDir = dir;
    }

    if (maxDistDog < dists[dir].dogDist) {
      maxDistDog = dists[dir].dogDist;
      maxDistDogDir = dir;
    }
  }

  int dir = 4;
  if (minDistSoul != INF) {
    dir = minDistSoulDir;
  } else {
    dir = maxDistDogDir;
  }

  if (dir == 4) return ds[dir];
  simulateWalk(id, dir);
  return ds[dir];
}


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

State genNextState(const State &nowState, const vector<string> &command){

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
    for (int i = 0; i < 5; i++){
      int nx = nextState.ninjas[id].x + dx[i];
      int ny = nextState.ninjas[id].y + dy[i];
      if (nextState.field[ny][nx].containsDog){
	flagFail = true;
	break;
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
  if (nowState.rivalSkillPoint < nowState.skillCount[2]){//unable fall rock
    return false;
  }
  int px = nowState.ninjas[id].x;
  int py = nowState.ninjas[id].y;
  int fallx = px + fallDx[fallDir];
  int fally = py + fallDy[fallDir];
  if (fallx <= 0 || fallx >= nowState.W - 1 || fally <= 0 || fally >= nowState.H - 1){//
    return false;
  }
  //  cerr << fallx << " " << fally << " " << nowState.field[fally][fallx].isNoCharacter() << endl;
  if (nowState.field[fally][fallx].isEmpty() && nowState.field[fally][fallx].isNoCharacter()){
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
void think(int depthLimit, int beamWidth=300) {
  
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
	  nextState.rivalSkillPoint -= nextState.skillCount[2];
	  currentState[0].push_back(nextState);
	}
      }
    }
  }
  
  vector<vector<string> > commands = createCommands();
  set<int> tabooCommands;
  //  cerr << currentState[0].size() << endl;
  for (int depth = 0; depth < depthLimit; depth++){
    sort(currentState[depth].rbegin(), currentState[depth].rend());
    if (currentState[depth].size() > beamWidth){
      currentState[depth].erase(currentState[depth].begin() + beamWidth, currentState[depth].end());
    }
    for (int i = 0; i < currentState[depth].size(); i++){
      State nowState = currentState[depth][i];
      if (tabooCommands.count(nowState.commandId) > 0){
	// cout << commands[nowState.commandId][0] << endl;
	// cout << commands[nowState.commandId][1] << endl;
	//	cout << nowState.commandId << endl;
	continue;
      }
      for (int j = 0; j < commands.size(); j++){
	State nextState = genNextState(nowState, commands[j]);
	if (nextState.fail){
	  
	  if (depth == 0){//taboo command;
	    //	    cout << "taboooo = "<< nextState.field[10][1].kind << endl;
	    tabooCommands.insert(j);
	  }
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
  for (int depth = depthLimit; depth >= 1; depth--){
    sort(currentState[depth].rbegin(), currentState[depth].rend());
    if (currentState[depth].empty())continue;
    //    cerr << "depth = " << depth << endl;
    //cerr << currentState[depth][0].getSoul << " " << depth << " " << currentState[depth][0].minDistSoulById[0] << " " << currentState[depth][0].minDistSoulById[1] << endl;
    int comId = currentState[depth][0].commandId;
    cout << 2 << endl;
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
  // AIの名前を出力
  cout << "TogaTogAI" << endl;
  cout.flush();

  while (input()) {
    think(3);
    cout.flush();
  }

  return 0;
}
