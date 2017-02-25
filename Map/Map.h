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
enum FieldCommand{COM_BOMB, COM_FLAG, COM_EMPTY, COM_VISIT};
struct Field {
    bool isBomb;
    bool isEmptySpace;
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
extern int usedFlags;

void prepareMapToPersist();
bool persistMap(ALLEGRO_DISPLAY * display);
bool loadMap(ALLEGRO_DISPLAY * display, std::string path);
void createFields();
void initializeEmptyMap(int sizeX, int sizeY);
void destroyMap();
void displayMap();
bool isGameFinished();
void setFieldAsBomb(int x, int y);
void setFieldAsEmpty(int x, int y);
void setFieldAsUnknown(int x, int y);
void setFieldAsFlagged(int x, int y);
void setFieldAsVirgin(int x, int y);
void setNextEditorState(Map *map, int x, int y);
void increaseNearbyFieldsFlags(int currentX, int currentY);
void decreaseNearbyFieldsFlags(int currentX, int currentY);
void changeNearbyFieldsFlags(int currentX, int currentY, int value);
void changeFieldValue(Field *field, int value);

void changeClickedFieldState(ALLEGRO_EVENT *event);

int getFieldSize();
std::vector<Map> getAllMaps();


#endif //MINESWEEPER_MAP_H
