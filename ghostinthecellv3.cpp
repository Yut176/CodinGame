#include<iostream>
#include<algorithm>
#include<cstdio>
#include<cstdlib>
#include<vector>
#include<string>
#include<sstream>
#include<cmath>
#include<numeric>
#include<queue>
int INF = 1000000000;
using namespace std;

int turn = 0;
int bombuse = 0;
int bombturn = 6;
class Edge{
public:
    int to, from, cost;
    void input(int a, int b, int c){
        to = a; from = b; cost = c;
    }
};
class Factory{
public:
    int entityId, player, cyborgs, production, reproductturn;
    vector<Edge> edges;
    void input(int id, int arg1, int arg2, int arg3, int arg4, int arg5){
        entityId = id; player = arg1; cyborgs = arg2; production = arg3; reproductturn = arg4;
    }
    void inputEdges(vector<Edge> e, int n){
        edges.resize(0);
        for(int i=0; i<e.size(); i++){
            if( e[i].from == n ){
                edges.push_back( e[i] );
            }
            if( e[i].to == n ){
                swap(e[i].to, e[i].from);
                edges.push_back( e[i] );
            }
        }
    }
};
class Troop{
public:
    int entityId, player, cyborgs, target, leaves, turn;
    void input(int id, int arg1, int arg2, int arg3, int arg4, int arg5){
        entityId = id; player = arg1; leaves = arg2; target = arg3; cyborgs = arg4; turn = arg5;
    }
};
class Bomb{
public:
    int player, factory, target, turn;
    void input(int arg1, int arg2, int arg3, int arg4, int arg5){
        player = arg1; factory = arg2; target = arg3; turn = arg4;
    }
};
class GameMap{
public:
    vector<Factory> factories; vector<Troop> troopers; vector<Bomb> bomb;
    int totalMyCyborgs = 0; int totalEnemyCyborgs = 0;
    vector<int> area;
    int mybomb = 2; int enemybomb = 2;
    void inputEdges(int f, int e){
        factories.resize(f);
        vector<Edge> edges(e);
        for(int i=0; i<e; i++){
            int a, b, c;
            cin >> a >> b >> c; cin.ignore();
            edges[i].input(a, b, c);
        }
        inputFactoryEdges(edges);
    }
    void inputFactoryEdges(vector<Edge> edges){
        for(int i=0; i<factories.size(); i++) factories[i].inputEdges( edges , i);
        for(int i=0; i<factories.size(); i++) cerr << factories[i].edges.size() << " ";
        cerr << endl;
    }
    void inputEntity(){
        troopers.resize(0);
        bomb.resize(0);
        int entityCount; // the number of entities (e.g. factories and troops)
        cin >> entityCount; cin.ignore();
        cerr << "-- INPUT --" << endl;
        for (int i = 0; i < entityCount; i++) {
            int entityId;
            string entityType;
            int arg1, arg2, arg3, arg4, arg5;

            cin >> entityId >> entityType >> arg1 >> arg2 >> arg3 >> arg4 >> arg5; cin.ignore();
            // cerr << " " << entityId << " " << entityType << " " << arg1 << " " << arg2 << " " << arg3 << " " << arg4 << " " << arg5 << " " << endl;
            if( entityType == "FACTORY" ){
                factories[ entityId ].input(entityId, arg1, arg2, arg3, arg4, arg5);
            }else if( entityType == "TROOP" ){
                Troop tmp;
                tmp.input(entityId, arg1, arg2, arg3, arg4, arg5);
                troopers.push_back(tmp);
            }else if( entityType == "BOMB" ){
                Bomb tmp;
                tmp.input(arg1, arg2, arg3, arg4, arg5);
                bomb.push_back(tmp);
            }
        }
        countUpCyborgs();
    }
    void countUpCyborgs(){
      totalMyCyborgs = 0;
      totalEnemyCyborgs = 0;
        for(int i=0; i<factories.size(); i++){
            if( factories[i].player == 1 ) totalMyCyborgs += factories[i].cyborgs;
            else if( factories[i].player == -1 ) totalEnemyCyborgs += factories[i].cyborgs;
        }
        for(int i=0; i<troopers.size(); i++){
            if( troopers[i].player == 1 ) totalMyCyborgs += troopers[i].cyborgs;
            else if( troopers[i].player == -1 ) totalEnemyCyborgs += troopers[i].cyborgs;
        }
    }
};
class Action{
public:
    string type;
    string msg;
    int source;
    int target;
    int cyborgs;
    void input(string s, int arg1, int arg2, int arg3){
        // input MOVE
        type = s;
        source = arg1;
        target = arg2;
        cyborgs = arg3;
    }
    void input(string s, string m){
        // input MSG
        type = s;
        msg = m;
    }
    void input(string s, int arg1){
        // input INC
        type = s;
        source = arg1;
    }
    void input(string s, int arg1, int arg2){
        // input BOMB
        type = s;
        source = arg1;
        target = arg2;
    }
    void output(){
        if( type == "MOVE" ) cout << type << " " << source << " " << target << " " << cyborgs;
        else if( type == "BOMB" ) cout << type << " " << source << " " << target;
        else if( type == "WAIT" ) cout << type;
        else if( type == "MSG" ) cout << type << " " << msg;
        else if( type == "INC" ) cout << type << " " << source;
    }
};

bool EdgesSort(Edge el, Edge er){
    if( el.cost > er.cost ) return false;
    else if( el.cost < er.cost ) return true;

    if( el.from > er.from ) return false;
    else if( el.from < er.from ) return true;

    return el.to > er.to;
}

// pos からの最小コストを返す (BFS)
vector<int> calcMinCost(GameMap g, int pos){
    vector<int> minCost(g.factories.size(), INF);

    // position cost
    queue< pair<int, int> > q;
    minCost[ pos ] = 0;
    q.push( make_pair(pos, 0) );
    while( !q.empty() ){
        pair<int, int> p = q.front();
        q.pop();
        vector<Edge> e = g.factories[ p.first ].edges;
        for(int i=0; i<e.size(); i++){
            if( p.first == e[i].from && p.second + e[i].cost < minCost[ e[i].to ] ){
                q.push( make_pair( e[i].to, p.second + e[i].cost ) );
                minCost[ e[i].to ] = p.second + e[i].cost;
            }
        }
    }
    return minCost;
}

GameMap inputArea(GameMap g){
    cerr << "-- INPUT AREA --" << endl;
    int myBase = 0;
    int enemyBase = 0;
    for(int i=0; i<g.factories.size(); i++){
        if( g.factories[i].player == 1 ) myBase = i;
        if( g.factories[i].player == -1 ) enemyBase = i;
    }
    g.area.resize(g.factories.size(), 0);
    vector<int> myCost = calcMinCost(g, myBase);
    vector<int> enemyCost = calcMinCost(g, enemyBase);

    for(int i=0; i<g.factories.size(); i++){
        if( myCost[i] == enemyCost[i] ) g.area[i] = 0;
        else if( myCost[i] > enemyCost[i] ) g.area[i] = -1;
        else if( myCost[i] < enemyCost[i] ) g.area[i] = 1;
    }
    for(int i=0; i<g.factories.size(); i++) cerr << g.area[i] << " ";
    cerr << endl;

    return g;
}

vector<int> nextFactoryMinCost(GameMap g, int pos, int target){
    vector<int> minCost = calcMinCost(g, pos);
    vector<int> path;
    int p = target;
    while( p != pos ){
        vector<Edge> e = g.factories[p].edges;
        for(int i=0; i<e.size(); i++){
            if( minCost[ e[i].to ] == minCost[ e[i].from ] - e[i].cost ){
                path.push_back( p );
                p = e[i].to;
                break;
            }
        }
    }
    path.push_back( p );

    return path;
}

string sendMoveMessege(int from, int to, int cyborgs){
    return "MOVE " + to_string( from ) + " " + to_string( to ) + " " + to_string( cyborgs );
}

string sendBomb(GameMap g, int target, bool shortest = true){
    cerr << "-- SEND BOMB --" << endl;
    cerr << " Target is " << target << endl;
    // vector<int> minCost = calcMinCost(g, target);
    vector< pair<int, int> > p;
    vector<Edge> edges = g.factories[target].edges;

    for(int i=0; i<edges.size(); i++){
        if( g.factories[ edges[i].to ].player != 1 ) continue;
        p.push_back( make_pair(edges[i].to, edges[i].from) );
    }
    sort(p.begin(), p.end());
    bombuse++;
    bombturn = 0;
    if( shortest ) return "BOMB " + to_string(p[0].first) + " " + to_string(p[0].second);
    else return "BOMB " + to_string(p[p.size()-1].first) + " " + to_string(p[p.size()-1].second);
}

vector<int> calcComeTrooper(GameMap g){
    vector<int> c(g.factories.size(), 0);
    for(int i=0; i<g.troopers.size(); i++){
        Troop tp = g.troopers[i];
        if( tp.player == -1 ) c[ tp.target ] += tp.cyborgs;
    }
    return c;
}

int decideTarget(GameMap g, int i){

    vector<Factory> f = g.factories;
    int mx = -INF;
    int con = 10;
    int target = 0;
    vector<int> comeTrooper = calcComeTrooper(g);
    for(int j=0; j<f[i].edges.size(); j++){
        if( f[ f[i].edges[j].to ].player == 1 && f[ f[i].edges[j].to ].cyborgs - comeTrooper[ f[i].edges[j].to ] > 0 ) continue;
        int fff = ( con - f[i].edges[j].cost ) * (f[ f[i].edges[j].to ].production+1);
        if( g.area[ f[i].edges[j].to ] == 1 ) fff += 30;
        if( g.factories[ f[i].edges[j].to ].player == 1 ) fff += 50;
        if( mx < fff ){
            mx = fff;
            target = f[i].edges[j].to;
        }
        cerr << f[i].edges[j].to << " " << fff << endl;
    }
    cerr << endl;
    cerr << i << " target is " << target << endl;

    return target;
}

vector<string> inFirstTurn(GameMap g){
    cerr << "-- inFirstTurn --" << endl;
    vector<string> msg;
    int n = g.factories.size();
    int myBase = 0;
    int enemyBase = 0;
    for(int i=0; i<n; i++){
        if( g.factories[i].player == 1 ) myBase = i;
        if( g.factories[i].player == -1 ) enemyBase = i;
    }


    vector<int> minCost = calcMinCost(g, enemyBase);

    int myCyborgs = g.factories[myBase].cyborgs;
    vector<Edge> e = g.factories[myBase].edges;
    // for(int i=0; i<e.size(); i++) cerr << e[i].to << " " << e[i].from << " " << e[i].cost << endl;
    sort(e.begin(), e.end(), EdgesSort);

    for(int i=0; i<e.size(); i++){
        // if( i != 0 ) continue;
        for(int prd = 3; prd>=1; prd--){
            if( g.factories[ e[i].to ].production != prd ) continue;
            if(myCyborgs >= 1 && myCyborgs >= g.factories[ e[i].to ].cyborgs + 1 && e[i].cost <= minCost[ e[i].to ]){
            // if(myCyborgs >= 1 && myCyborgs >= g.factories[ e[i].to ].cyborgs + 1 ){
                msg.push_back( sendMoveMessege(myBase, e[i].to, g.factories[ e[i].to ].cyborgs + 1) );
                myCyborgs -= g.factories[ e[i].to ].cyborgs + 1;
            }
        }
    }
    return msg;
}

vector<string> AIv1(GameMap g){
    cerr << "-- AI --" << endl;
    vector<string> msg;
    int n = g.factories.size();
    vector<Factory> f = g.factories;

    for(int i=0; i<n; i++){
        if( f[i].player == 0 ){
            continue;
        }if( f[i].player == -1 ){
            if( g.totalEnemyCyborgs == 0 ) continue;
            double ppp = f[i].cyborgs / g.totalEnemyCyborgs;
            if( f[i].production == 3 && bombuse < 2 && bombturn > 5 ) msg.push_back( sendBomb(g, i, true) );
            // continue;
        }else if( f[i].production == 0 && f[i].cyborgs > 0 ){
            if( turn > 5 && f[i].cyborgs > 10 ) msg.push_back("INC " + to_string(i) );
            else if(turn < 10){
                // 生産量が 0 のとき最近点へ全移動
                int mn = INF;
                int target = 0;
                for(int j=0; j<f[i].edges.size(); j++){
                    if( f[ f[i].edges[j].to ].player == -1 ) continue;
                    if( f[ f[i].edges[j].to ].production == 0 ) continue;
                    if( mn > f[i].edges[j].cost ){
                        mn = f[i].edges[j].cost;
                        target = f[i].edges[j].to;
                    }
                }
                if( i != target ) msg.push_back( sendMoveMessege(i, target, f[i].cyborgs) );
            }
        }else if( f[i].production > 0 ){
            if( turn > 5 && f[i].player == 1 && f[i].production < 3 && 10 + f[i].production * 2 <= f[i].cyborgs ){
            // if( turn > 10 && f[i].player == 1 && f[i].production < 3 && g.totalEnemyCyborgs / 10 + 20 < f[i].cyborgs ){
                msg.push_back("INC " + to_string(i) );
                f[i].cyborgs -= 10;
            }

            int target = decideTarget(g, i);
            vector<int> t = nextFactoryMinCost(g, i, target);
            for(int j=t.size()-1; j>=0; j--) cerr << " " << t[j];
            cerr << endl;

            if( i != target ){
                int product = f[i].edges[target].cost * f[target].production;
                for(int j=t.size()-1; j>0; j--){
                    if( f[ t[j] ].player != 1 ) break;
                    if( min(f[t[j]].cyborgs, f[i].cyborgs)-2 <= 0 ) continue;
                    if( f[ t[j] ].production < 3 ){
                        if( f[ t[j] ].production-1 <= 0 ) continue;
                        msg.push_back( sendMoveMessege(t[j], t[j-1], f[ t[j] ].production-1 ) );
                    }else msg.push_back( sendMoveMessege(t[j], t[j-1], min(f[t[j]].cyborgs, f[i].cyborgs)) );
                }
            }


        }
    }
    return msg;
}

void Conduct(GameMap gamemap){
    // SearchOne(gamemap);
    vector<string> s;

    if(turn == 1) s = inFirstTurn(gamemap);
    else s = AIv1(gamemap);

    AIv2(gamemap);

    // else s = AIv2(gamemap);
    cerr << "-- ACTION --" << endl;

    if( s.size() == 0 ) cout << "WAIT" << endl;
    else if( s.size() == 1 ) cout << s[0] << endl;
    else {
        for(int i=0; i<s.size(); i++){
            if( i == s.size()-1 ) cerr << s[i] << endl;
            else cerr << s[i] << ";";
            if( i == s.size()-1 ) cout << s[i] << endl;
            else cout << s[i] << ";";
        }
    }
}


int main(){
    int factoryCount; // the number of factories
    cin >> factoryCount; cin.ignore();
    int linkCount; // the number of links between factories
    cin >> linkCount; cin.ignore();

    GameMap gamemap;
    // gamemap.inputAllEdges(linkCount);
    gamemap.inputEdges(factoryCount, linkCount);
    bool fs = true;
    // game loop
    while (1) {
        gamemap.inputEntity();
        if( fs ){
            gamemap = inputArea(gamemap);
            fs = false;
        }
        bombturn++;
        turn++;
        cerr << "Turn is " << turn << endl;

        Conduct(gamemap);
    }
}











// EOF
