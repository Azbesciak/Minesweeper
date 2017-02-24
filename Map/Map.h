//
// Created by witek on 21.02.17.
//

#ifndef MINESWEEPER_MAP_H
#define MINESWEEPER_MAP_H
#include "../Utils/Utils.h"
#include <allegro5/allegro5.h>
#include <string>
#include <vector>
const std::string MAPS_PATH = "../resources/maps/";
const double FIELD_SPAN_RATIO = 0.05;
enum FieldType{FIELD_BOMB, FIELD_EMPTY, FIELD_UNKNOWN, FIELD_FLAG, FIELD_NEST, FIELD_WATER};

struct Field {
    FieldType type;
    bool wasVisited;
    bool isFlagged;
    int bombsInArea;
};

struct Map {
    Field **fields;
    int sizeX;
    int sizeY;
    int bombsLimit;
    int possibleToVisit;
    std::string name;
};

bool persistMap(Map *map);
Map loadMap(ALLEGRO_DISPLAY * display, std::string path);
Map initializeEmptyMap(int sizeX, int sizeY);
void displayMap(Map *map);
bool updateGameState(Map *map);

int getFieldSize(Map *map);
std::vector<Map> getAllMaps();


#endif //MINESWEEPER_MAP_H
