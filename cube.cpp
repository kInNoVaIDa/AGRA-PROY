#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
using namespace std;
int orientations[6][6][4];

struct State {
    int pos;
    int packedori;
    int ifgold;
    long long goldpos;

    State(int p, int ori, int ifg,long long gp) {
        pos = p, packedori = ori, ifgold = ifg, goldpos = gp;
    }
    
    void addGoldtoFace(int posgold, int C) {
        int bottom = packedori & 7;
        ifgold = ifgold | (1 << bottom);
        
        int j = posgold & 7;
        int i = (posgold >> 3) & 7; 
        goldpos = goldpos & ~((long long)1 <<((i*C) + j));
    }
    void subGoldfromFace(int C) {
        int bottom = packedori & 7;
        ifgold = ifgold & ~(1 << bottom);
        
        int j = pos & 7;
        int i = (pos >> 3) & 7;
        goldpos = goldpos | ((long long)1 << ((i * C) + j));
    }

    int move(int dir) {
        int bottom = packedori & 7;
        int front = (packedori >> 3) & 7;

        return orientations[bottom][front][dir];
    }
    
    bool operator==(const State &b) const {
        return (pos == b.pos) && (packedori == b.packedori)
        && (goldpos == b.goldpos);
    }
    bool operator<(const State &b) const {
        return pos < b.pos;
    }
};
namespace std {
    template<>
    struct hash<State> {
        size_t operator()(const State &a) const noexcept {
            size_t result = 0;
            result ^= hash<int>{}(a.pos) + (result << 6) + (result >> 2);
            result ^= hash<int>{}(a.packedori) + (result << 6) + (result >> 2);
            result ^= hash<int>{}(a.ifgold) + (result << 6) + (result >> 2);
            result ^= hash<long long>{}(a.goldpos) + (result << 6) + (result >> 2);
            return result;
        }
    };
}
unordered_map<State, int> dist;
vector<int> dr = {1, -1, 0, 0};
vector<int> dc = {0, 0, -1, 1};

int packOri(int bottom, int front, int east, int west, int top, int back) {
    return (back << 15) | (top << 12) | (west << 9) | (east << 6) | (front << 3) | bottom;
}

int moveCube(int packedori, int dir) {
        int bottom = packedori & 7;
        int front = (packedori >> 3) & 7;
        int east = (packedori >> 6) & 7;
        int west = (packedori >> 9) & 7;
        int top = (packedori >> 12) & 7;
        int back = (packedori >> 15) & 7;

        int aux;
        if (dir == 0) {
            aux = back;
            back = bottom;
            bottom = front;
            front = top;
            top = aux; 
        } else if (dir == 1) {
            aux = front;
            front = bottom;
            bottom = back;
            back = top;
            top = aux;
        } else if (dir == 2) {
            aux = west;
            west = bottom;
            bottom = east;
            east = top;
            top = aux;
            
        } else if (dir == 3) {
            aux = east;
            east = bottom;
            bottom = west;
            west = top;
            top = aux;
        }
        return (back << 15) | (top << 12) | (west << 9) | (east << 6) | (front << 3) | bottom;
}   


int collectAllGolds(State init, int R, int C, int A, int B) {
    dist.clear();
    dist.reserve(3000000);

    using rename_state = pair<int, State>;
    priority_queue<rename_state, vector<rename_state>, greater<rename_state>> pq;
    dist[init] = 0;
    pq.push({0, init});
    
    int ans = -1;
    while (!pq.empty() && ans == -1) {
        rename_state top = pq.top();
        pq.pop();
        int cost = top.first;
        State initState = top.second;
        if (cost == dist[initState]) {
            if (initState.ifgold != 63) {
                int newc, newr;
                int c = initState.pos & 7;
                int r = initState.pos >> 3;
                for (int d = 0; d < 4; d++) {

                    newc = c + dc[d];
                    newr = r + dr[d];
                    if (newc >= 0 && newc < C && newr >= 0 && newr < R) {
                        int newposcube = newr << 3 | newc;
                        State newState(newposcube, initState.move(d), initState.ifgold, initState.goldpos);

                        int cellhasgold = -1;
                        if (((newState.goldpos >> ((newr*C) + newc)) & 1) == 1) {
                            cellhasgold = newposcube;
                        }

                        int newcost;
                        int bottom = newState.packedori & 7;
                        bool facesHasgold = (newState.ifgold & (1 << bottom)) != 0;
                        // cara sin oro y celda sin oro costo A
                        if (!facesHasgold && cellhasgold == -1) {
                            newcost = A + cost;
                        //cara sin oro y celda con oro costo B
                        } else if (!facesHasgold && cellhasgold != -1) {
                            newState.addGoldtoFace(cellhasgold, C);
                            newcost = B + cost;
                        // cara con oro y celda con oro costo A
                        } else if (facesHasgold && cellhasgold != -1) {
                            newcost = A + cost;
                            // cara con oro y celda sin oro A
                        } else if (facesHasgold && cellhasgold == -1) {
                            newState.subGoldfromFace(C);
                            newcost = A + cost;
                        }

                        if (dist.count(newState) == 0 || newcost < dist[newState]) {
                                dist[newState] = newcost;
                                pq.push({newcost, newState});
                        }
                    }
                }
            } else {
                ans = cost;
            }
        }
    }

    return ans;
}


void calculateOrientations() {
    unordered_map<int, bool> visited;
    queue<int> pq;
    int initOri = packOri(0,1,2,3,4,5);
    visited[initOri] = true;
    pq.push(initOri);
    
    int u;
    while (!pq.empty()) {
        u = pq.front();
        pq.pop();
        int bottom = u & 7;
        int front = (u >> 3) & 7;
        int v;

        for (int d = 0; d < 4; d++) {
            v = moveCube(u, d);
            int nbottom = v & 7;
            int nfront = (v >> 3) & 7;  
            orientations[bottom][front][d] = (nfront << 3) | nbottom;

            if (visited.count(v) == 0) {
                visited[v] = true;
                pq.push(v);
            }            
        }
    }

}

int main() {
    calculateOrientations();
    int T, R, C, A, B;
    cin >> T;

    while (T--) {
        cin >> R >> C >> A >> B;
        vector<vector<char>> map(R, vector<char>(C));
        string row;

        int i = 0, j;
        int cpos;
        long long goldpos = 0;
        int shift = 0;
        while (i < R) {
            cin >> row;
            j = 0;
            while (j < C) {
                map[i][j] = row[j];
                if (map[i][j] == 'G') {
                    goldpos = goldpos | ((long long)1 << ((i*C) + j));
                    ++shift;
                } else if (map[i][j] == 'S') {
                    cpos = i << 3 | j;
                }
                ++j;
            }
            ++i;
        }
        int initori = (1 << 3) | 0;
        State initState(cpos, initori, 0, goldpos);

        int result = collectAllGolds(initState, R, C, A, B);
        if (result == -1){
            cout << "Oh my God, they killed Kenny!" << endl;
        } else {
            cout << "Screw you guys, I got all the gold for "<< result << " cost!" << endl;
        }
    }
    return 0;
}