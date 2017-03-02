//
// Created by witek on 21.02.17.
//

#ifndef MINESWEEPER_MAP_H
#define MINESWEEPER_MAP_H

#include "../../Utils/Utils.h"
#include "../../Game/States/GameStates.h"
#include <allegro5/allegro5.h>
#include <string>
#include <vector>
#include <allegro5/allegro_font.h>

const std::string MAPS_PATH = "../resources/maps/";
const double FIELD_SPAN_RATIO = 0.05;
enum FieldType {
    FIELD_NORMAL, FIELD_BOMB, FIELD_EMPTY_SPACE
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
};

extern Map *map;
extern int fieldSize;
extern int fieldWithSpanSize;
extern int startX;
extern int startY;
extern GameState gameState;
extern Point *lastClick;

void prepareMapToPersist();

bool persistMap(ALLEGRO_DISPLAY *display);

bool selectMap(bool isCustom, ALLEGRO_DISPLAY *display);

bool loadMap(ALLEGRO_DISPLAY *display, std::string path);

string getMapProps();

void createFields();

void initializeEmptyMap(int sizeX, int sizeY);

void destroyMap();

void displayMap(bool isGame);

void drawField(int x, int y, Field *field, int fieldWithSpanSize, bool showBombsInfo);

void drawSmallFieldWithColor(int x, int y, ALLEGRO_COLOR color);

void drawFieldWithColor(int x, int y, ALLEGRO_COLOR color);

int getFieldSize();


#endif //MINESWEEPER_MAP_H
