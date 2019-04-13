#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

#define  MAP_SIZE 20

/* Calculate square distance between (x, y) and (t_x, t_y) */
inline float doubled_distance(float x, float y, float t_x, float t_y){
    return (float)(x-t_x)*(x-t_x) + (y-t_y)*(y-t_y); 
}

/* Calculate field value at (x, y) relate to (t_x, t_y) */
inline float field_calc(int x, int y, float t_x, float t_y, float mass){
    return  mass/doubled_distance((float)x, (float)y, t_x, t_y);
}

/* Update map with specified mass and point */
template <size_t size_x, size_t size_y>
void field_accum(float (&fmap)[size_x][size_y], int size, float x, float y, float mass){
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            fmap[i][j] += field_calc(i, j, x, y, mass);
        }
    }
}

/* Print the map with number table */
template <size_t size_x, size_t size_y>
void print_map(float (&fmap)[size_x][size_y], int size){
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            cout << setw(2) << setprecision(1) << fmap[i][j] << '\t';
        }
        cout << endl;
    }
}

/* Init the map with specified value */
template <size_t size_x, size_t size_y>
void map_init(float (&fmap)[size_x][size_y], int size, int value){
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            fmap[i][j] = 0;
        }
    }
}

int main(){
    float map[MAP_SIZE][MAP_SIZE];
    map_init(map, MAP_SIZE, 0);
    float x, y, m;
    cin >> x >> y >> m;
    field_accum(map, MAP_SIZE, x, y, m);
    print_map(map, MAP_SIZE);
}
