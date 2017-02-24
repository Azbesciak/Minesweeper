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
enum FieldType{FIELD_BOMB, FIELD_EMPTY, FIELD_UNKNOWN, FIELD_FLAG, FIELD_WATER, FIELD_VIRGIN};
const int MAP_FIELDS_TYPES = 6;

struct MapColors {
    ALLEGRO_COLOR *colors;
};

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
    int emptyFields;
    int fieldSize;
    std::string name;
};
void initializeColors(MapColors *palette);
void destroyColors(MapColors *colors);
bool persistMap(Map *map);
bool loadMap(Map *map, ALLEGRO_DISPLAY * display, std::string path);
void createFields(Map *map);
void initializeEmptyMap(Map *map, int sizeX, int sizeY);
void destroyMap(Map *map);
void displayMap(Map *map);
bool updateGameState(Map *map);
void setFieldAsBomb(Map *map, int x, int y);
void setFieldAsEmpty(Map *map, int x, int y);
void setFieldAsUnknown(Map *map, int x, int y);
void setFieldAsFlagged(Map *map, int x, int y);
void setFieldAsVirgin(Map *map, int x, int y);

void setFieldType(Map *map, int x, int y, FieldType type);

int getFieldSize(Map *map);
std::vector<Map> getAllMaps();


#endif //MINESWEEPER_MAP_H
