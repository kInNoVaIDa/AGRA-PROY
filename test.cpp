#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
using namespace std;

/*
    Corregir representación de estados de forma empaquetada, empaquetar
    la cara tomando en cuenta las potencias de 2 como indices
    sub mascara para saber que cara tiene oro y por ultimo otra submascara

    corregir la forma de representar el diccionario de golds
    y terminar el estruct del cubo y modificar el codigo para que use el struct del cubo
*/
// estado
struct State {
    int pos;
    int packedori;
    int ifgold;
    long long goldonfaces;
    long long goldpos;

    State(int p, int ori, int ifg, long long gof, long long gp) {
        pos = p, packedori = ori, ifgold = ifg, goldonfaces = gof,  goldpos = gp;

    }
    

    bool operator==(const State &b) const {
        return (pos == b.pos) && (packedori == b.packedori)
        && (ifgold == b.ifgold) && (goldonfaces == b.goldonfaces)
        && (goldpos == b.goldpos);
    }
    bool operator<(const State &b) const {
        return pos < b.pos;
    }
};
//hash para el struct
namespace std {
    template<>
    struct hash<State> {
        size_t operator()(const State &a) const noexcept {
            auto combineHash = [](size_t &seed, size_t hashValue) {
                seed ^= hashValue + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            };

            size_t result = 0;
            combineHash(result, hash<int>{}(a.pos));
            combineHash(result, hash<int>{}(a.packedori));
            combineHash(result, hash<int>{}(a.ifgold));
            combineHash(result, hash<long long>{}(a.goldonfaces));
            combineHash(result, hash<long long>{}(a.goldpos));

            return result;
        }
    };
}
unordered_map<State, int> dist;
vector<int> dr = {1, -1, 0, 0};
vector<int> dc = {0, 0, -1, 1};
// north, souht, east, west

int packOri(int bottom, int front, int east, int west, int top, int back) {
    return (back << 15) | (top << 12) | (west << 9) | (east << 6) | (front << 3) | bottom;
}
vector<int> unPackOri(int packedori) {
        vector<int> faces(6);
        faces[0] = packedori & 7;
        faces[1] = (packedori >> 3) & 7;
        faces[2] = (packedori >> 6) & 7;
        faces[3] = (packedori >> 9) & 7;
        faces[4] = (packedori >> 12) & 7;
        faces[5] = (packedori >> 15) & 7;

        return faces;
}
    int getGoldonFace(long long gof, int face) {
        return (gof >> (face * 7)) & 127;
    }
void addGoldtoface(int &ifgold, long long &goldonfaces, int posgold, int bottom, long long &goldpos, int C) {
    ifgold = ifgold | (1 << bottom);
    goldonfaces = goldonfaces & ~((long long)127 << (bottom * 7));
    goldonfaces = goldonfaces | ((long long)posgold << (bottom * 7));
    int j = posgold & 7;
    int i = (posgold >> 3) & 7; 
    goldpos = goldpos & ~((long long)1 <<((i*C) + j));
}
void subGoldfromface(int &ifgold, long long &goldonfaces, int bottom, int poscube, long long &goldpos, int C) {
    ifgold = ifgold & ~(1 << bottom);
    goldonfaces = (goldonfaces & ~((long long)127 << bottom * 7)) | ((long long)64 << bottom*7);
    int j = poscube & 7;
    int i = (poscube >> 3) & 7;
    goldpos = goldpos | ((long long)1 << ((i * C) + j));

}
int moveCube(int packedori, int dir) {
    vector<int> faces = unPackOri(packedori);
    int bottom = faces[0];
    int front = faces[1];
    int east = faces[2];
    int west = faces[3];
    int top = faces[4]; 
    int back = faces[5];
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
    return packOri(bottom, front, east, west, top, back);
}

int collectAllGolds(State init, int R, int C, int A, int B) {
    dist.clear();
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
            //info del cubo y u es el estado
            int bottom = initState.packedori & 7;
            int packedori = initState.packedori;
            int ifgold = initState.ifgold ;
            int poscube = initState.pos;
            long long goldonfaces = initState.goldonfaces;
            long long goldpos = initState.goldpos;
            //columna y fila en la que se encuentra el cubo
            int c = poscube & 7;
            int r = poscube >> 3;

            if (ifgold != 63) {
                for (int d = 0; d < 4; d++) {
                    // nuevas coordenadas de pos pal cubo
                    int newc = c + dc[d];
                    int newr = r + dr[d];
                    
                    if (newc >= 0 && newc < C && newr >= 0 && newr < R) {
                        int newpackedOri = moveCube(packedori, d);
                        int newbottom = newpackedOri & 7;
                        int newifgold = ifgold;
                        int newposcube = newr << 3 | newc;
                        long long newgoldonfaces = goldonfaces;
                        long long newgoldpos = goldpos;

                        int cellhasgold = -1;
                        if (((newgoldpos >> ((newr*C) + newc)) & 1) == 1) {
                            cellhasgold = newposcube;
                        }


                        int newcost;
                        int goldonface = getGoldonFace(newgoldonfaces, newbottom);
                        // cara sin oro y celda sin oro costo A
                        if (goldonface == 64 && cellhasgold == -1) {
                            //creamos el nuevo estado
                            State newState(newposcube, newpackedOri, newifgold, newgoldonfaces, newgoldpos);
                            newcost = A + cost;
                            //posible cambio de la condicion
                            if (dist.count(newState) == 0 || newcost < dist[newState]) {
                                dist[newState] = newcost;
                                pq.push({newcost, newState});
                            }
                        //cara sin oro y celda con oro costo B
                        } else if (goldonface == 64 && cellhasgold != -1) {
                            addGoldtoface(newifgold, newgoldonfaces, cellhasgold, newbottom, newgoldpos, C);
                            State newState(newposcube, newpackedOri, newifgold, newgoldonfaces, newgoldpos);
                            newcost = B + cost;

                            if (dist.count(newState) == 0 || newcost < dist[newState]) {
                                dist[newState] = newcost;
                                pq.push({newcost, newState});
                            }
                        // cara con oro y celda con oro costo A
                        } else if (goldonface != 64 && cellhasgold != -1) {
                            State newState(newposcube, newpackedOri, newifgold, newgoldonfaces, newgoldpos);
                            newcost = A + cost;

                            if (dist.count(newState) == 0 || newcost < dist[newState]) {
                                dist[newState] = newcost;
                                pq.push({newcost, newState});
                            }

                            // cara con oro y celda sin oro A
                        } else if (goldonface != 64 && cellhasgold == -1) {
                            subGoldfromface(newifgold, newgoldonfaces, newbottom, newposcube, newgoldpos, C);
                            newcost = A + cost;
                            State newState(newposcube, newpackedOri, newifgold, newgoldonfaces, newgoldpos);

                            if (dist.count(newState) == 0 || newcost < dist[newState]) {
                                dist[newState] = newcost;
                                pq.push({newcost, newState});
                            }
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

int main() {
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
        int initori = packOri(0,1,2,3,4,5);
        long long initGof = ((long long)64 << 35) | ((long long)64 << 28) | ((long long)64 << 21) | ((long long)64 << 14) | ((long long)64 <<  7) | (long long)64;
        State initState(cpos, initori, 0, initGof, goldpos);
        int result = collectAllGolds(initState, R, C, A, B);
        cout << result << endl;
    }
    return 0;
}