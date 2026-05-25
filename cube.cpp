#include <iostream>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <queue>
#include <map>
using namespace std;

/*
    Corregir representación de estados de forma empaquetada, empaquetar
    la cara tomando en cuenta las potencias de 2 como indices
    sub mascara para saber que cara tiene oro y por ultimo otra submascara

    corregir la forma de representar el diccionario de golds
    y terminar el estruct del cubo y modificar el codigo para que funcione
*/

map<pair<long long, long long>, int> dist;
vector<int> dr = {1, -1, 0, 0};
vector<int> dc = {0, 0, -1, 1};
// unordered_map<int,int> opposite = {
//     {1, 32},
//     {2, 16},
//     {4, 8}
// };
unordered_map<int, bool> faceID = {
    {1,0}, {2,1}, {4, 2}, {8, 3}, {16, 4}, {32, 5}
};
// funcion solo para revisar el mapa
void printM(vector<vector<char>> &M) {
    for (auto &R: M) {
        cout << "[";
        for (auto &C: R) {
            cout << C << ", ";
        }
        cout << "]" << endl;
    }
}

struct Orientation {
    int bottom;
    int front;
    int east;
    int west; 
    int top;
    int back;
};
long long mixInf(int face, int gcube, int faceg) {
    long long fg = 0;
    for (int i = 0; i < 6; i++) {
        fg = fg | ((long long)15) << (i*4);
    }

    return ((long long) face << 36) | (long long)gcube << 30 | (long long)faceg << 24 | fg;
}
int getbottom(long long s) {
    return 1 << ((s >> 36) & 7);
} 
int getgCube(long long s) {
    return ((s >> 30) & 63);
}
int getFaceg(long long s) {
    return ((s >> 24) & 63);
}
int getGoldonFace(long long s, int face) {
    return (s >> (face * 4)) & 15;
}

long long makeState(int nbottombit, int gcube, int faceg, long long antS, int antF, int goldidx) {
    long long fg = antS & 0xFFFFFF;
    fg = fg & ~((long long)15 << (antF*4));
    fg = fg | ((long long)(goldidx & 15)) << (antF * 4);
    return ((long long)nbottombit << 36) | ((long long)gcube << 30) | ((long long)faceg << 24) | fg;
}


struct Cube {
    long long pos;
    int bottom;
    int gold;
    int faceG;
};

int moveCube(int face, int dir, Orientation &ori) {
    int aux;
    if (dir == 0) {
        aux = ori.back;
        ori.back = face;
        ori.bottom = ori.front;
        ori.front = ori.top;
        ori.top = aux; 
    } else if (dir == 1) {
        aux = ori.front;
        ori.front = face;
        ori.bottom = ori.back;
        ori.back = ori.top;
        ori.top = aux;
    } else if (dir == 2) {
        aux = ori.west;
        ori.west = face;
        ori.bottom = ori.east;
        ori.east = ori.top;
        ori.top = aux;
    } else if (dir == 3) {
        aux = ori.east;
        ori.east = face;
        ori.bottom = ori.west;
        ori.west = ori.top;
        ori.top = aux;
    }

    return ori.bottom;
}

int collectAllGold(pair<long long, long long> &init, int R, int C,int A, int B, Orientation &ori, vector<long long> golds) {
    dist.clear();
    using State = pair<int, pair<long long, long long>>;
    priority_queue<State, vector<State>, greater<State>> pq;
    dist[init] = 0;
    pq.push({0, init});

    int du;
    pair<long long, long long> u;
    bool done = true;
    while (!pq.empty() && done) {
        State top = pq.top();
        pq.pop();
        du = top.first, u = top.second;

        if (du == dist[u]) {
            long long pos = u.first;
            int bottom = getbottom(u.second);
            int gcube = getgCube(u.second);
            int faceg = getFaceg(u.second);
            int col = pos & 7;
            int row = pos >> 3;

            if (faceg != 63) {
                for (int m = 0; m < 4; m++) {
                int nr = row + dr[m];
                int nc = col + dc[m];

                    if (nr >= 0 && nr < R && nc >= 0 && nc < C) {
                        Orientation tmp = ori;
                        long long npos = (long long)nr << 3 | nc;
                        int nbottom = moveCube(bottom, m, tmp);
                        int nbottombit = faceID[nbottom];
                        int ngcube = gcube;
                        int nfaceg = faceg;


                        int goldidx = -1, i = 0;
                        bool flag = true;
                        while (i < 6 && flag) {
                            // en caso de que me vaya a mover a una celda con oro y no haya sido recogido ese oro
                            if (npos == golds[i] && !(ngcube & (1 << i))) {
                                goldidx = i;
                                flag = false;
                            }
                            ++i;
                        }

                        int cost;
                        bool cellhasgold = goldidx != -1;
                        bool facehasgold = (nfaceg & (1 << nbottombit)) == 1;
                        if (cellhasgold && !facehasgold) {
                            ngcube = ngcube | (1 << goldidx);
                            nfaceg = nfaceg | (1 << nbottombit);
                            cost = B;

                            long long nfgm = makeState(nbottombit, ngcube, nfaceg, u.second, nbottombit, goldidx);
                            pair<long long, long long> nState = {npos, nfgm};
                            int ndu = du + cost;
                            if (dist.count(nState) == 0 || ndu < dist[nState]) {
                                dist[nState] = ndu;
                                pq.push({ndu, nState});
                            }
                        } else if (!cellhasgold && facehasgold) {
                            int gold = getGoldonFace(u.second, nbottombit);
                            ngcube = ngcube & ~(1 << gold);
                            nfaceg = nfaceg & ~(1 << nbottombit);
                            golds[gold] = npos;
                            cost = A;
                            
                            long long nfgm = makeState(nbottombit, ngcube, nfaceg, u.second, nbottombit, 15);
                            pair<long long, long long> nState = {npos, nfgm};
                            int ndu = du + cost;
                            if (dist.count(nState) == 0 || ndu < dist[nState]){
                                dist[nState] = ndu;
                                pq.push({ndu, nState});
                            }
                        } else {
                            cost = A;
                            int gold = getGoldonFace(u.second, nbottombit);
                            long long nfgm = makeState(nbottombit, ngcube, nfaceg, u.second, nbottombit, gold);
                            pair<long long, long long> nState = {npos, nfgm};
                            int ndu = du + cost;
                            if (dist.count(nState) == 0 || ndu < dist[nState]) {
                                dist[nState] = ndu;
                                pq.push({ndu, nState});
                            }
                        }
                    }
                }
            } else {
                done = false;
            }
        }
    }
    if (!done) {
        du = -1;
    }
    return du;
}

int main() {
    int T, R, C, A, B;
    cin >> T;

    while (T--) {
        cin >> R >> C >> A >> B;
        vector<vector<char>> map(R, vector<char>(C));
        vector<long long> golds;
        string row;
        int i = 0, j;
        long long cpos;

        while (i < R) {
            cin >> row;
            j = 0;
            while (j < C) {
                map[i][j] = row[j];
                if (map[i][j] == 'G') {
                    long long pos = i << 3 | j;
                    golds.push_back(pos);
                } else if (map[i][j] == 'S') {
                    cpos = i << 3 | j;
                }
                ++j;
            }
            ++i;
        }
        Orientation orientation = {1, 2, 8, 4, 32, 16};
        long long fgm = mixInf(0,0,0);
        pair<long long, long long> state = {cpos, fgm};
        // printM(map);
        int result = collectAllGold(state, R, C, A, B, orientation, golds);

        cout << result << endl;
         break;
    }


    return 0;
}