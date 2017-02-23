//
// Created by witek on 21.02.17.
//

#ifndef MINESWEEPER_MAP_H
#define MINESWEEPER_MAP_H
#include "../Utils/Utils.h"

#include <string>
#include <vector>

struct Map{
    Field fields[100][100];
    int maxX;
    int maxY;
    std::string name;
};

bool persistMap(Map *map, std::string *name);
Map readMap(std::string name);
Map initializeEmptyMap(int maxX, int maxY);
void displayMap(Map *map);
std::vector<Map> getAllMaps();


#endif //MINESWEEPER_MAP_H
