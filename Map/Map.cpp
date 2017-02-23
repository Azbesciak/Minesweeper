//
// Created by witek on 21.02.17.
//
#include <iostream>
#include <fstream>
#include "Map.h"

using namespace std;
bool persistMap(Map *map, string name) {
    string temp = "../resources/maps/" + name + ".map";

    fstream toSave;
    toSave.open(temp.c_str(), ios::out|ios::trunc);
    if (!toSave.good()) return false;


}
Map readMap(string name) {

}


Map initializeEmptyMap(int maxX, int maxY);
void displayMap(Map *map);
vector<Map> getAllMaps();
