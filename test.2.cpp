// #include <iostream>
// #include <vector>
// #include <queue>
// #include <unordered_map>
// using namespace std;

// vector<int> dr = {1, -1, 0, 0};
// vector<int> dc = {0, 0, -1, 1};
// int packOri(int bottom, int front, int east, int west, int top, int back) {
//     return (back << 15) | (top << 12) | (west << 9) | (east << 6) | (front << 3) | bottom;
// }

// int moveCube(int packedori, int dir) {
//     int bottom = packedori & 7;
//     int front = (packedori >> 3) & 7;
//     int east = (packedori >> 6) & 7;
//     int west = (packedori >> 9) & 7;
//     int top = (packedori >> 12) & 7;
//     int back = (packedori >> 15) & 7;

//     int aux;
//     if (dir == 0) {
//         aux = back;
//         back = bottom;
//         bottom = front;
//         front = top;
//         top = aux; 
//     } else if (dir == 1) {
//         aux = front;
//         front = bottom;
//         bottom = back;
//         back = top;
//         top = aux;
//     } else if (dir == 2) {
//         aux = west;
//         west = bottom;
//         bottom = east;
//         east = top;
//         top = aux;
        
//     } else if (dir == 3) {
//         aux = east;
//         east = bottom;
//         bottom = west;
//         west = top;
//         top = aux;
//     }
//         return (back << 15) | (top << 12) | (west << 9) | (east << 6) | (front << 3) | bottom;
// }

// unordered_map<int, bool> visited;
// int orientations[6][6];



// int main() {

//     int initori = packOri(0,1,2,3,4,5);
//     bfs(initori);
//     cout << orientations << endl;




//     return 0;
// }