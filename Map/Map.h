//
// Created by witek on 21.02.17.
//

#ifndef MINESWEEPER_MAP_H
#define MINESWEEPER_MAP_H
#include "../Utils/Utils.h"
#include <allegro5/allegro5.h>
#include <string>
#include <vector>
#include <allegro5/allegro_font.h>

const std::string MAPS_PATH = "../resources/maps/";
const double FIELD_SPAN_RATIO = 0.05;
enum FieldCommand{COM_BOMB, COM_FLAG, COM_EMPTY, COM_VISIT};
enum FieldType{FIELD_NORMAL, FIELD_BOMB, FIELD_EMPTY_SPACE};
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
extern int usedFlags;
extern int score;
extern int timer;
extern bool isGame;
extern ALLEGRO_FONT *fieldsFont;
extern ALLEGRO_FONT *statsFont;

void prepareMapToPersist();
bool persistMap(ALLEGRO_DISPLAY * display);
bool selectMap(bool isCustom, ALLEGRO_DISPLAY *display);
bool loadMap(ALLEGRO_DISPLAY * display, std::string path);
void createFields();
void initializeEmptyMap(int sizeX, int sizeY);
void destroyMap();
void displayMap(bool isGame);
void readUserOnMapClick(ALLEGRO_EVENT *event, void(*f)(int, int));
void resetScore();
void resetTimer();
void incrementTimer();
void maintainGame(ALLEGRO_EVENT *event);
void maintainEditor(ALLEGRO_EVENT *event);
bool isGameFinished();
void setBombsLimit(bool isSaveState);
void setFieldAsBomb(int x, int y);
void setFieldAsEmpty(int x, int y);
void setFieldAsUnknown(int x, int y);
void setFieldAsFlagged(int x, int y);
void setFieldAsVirgin(int x, int y);
void setNextEditorState(int x, int y);
void increaseNearbyFieldsFlags(int currentX, int currentY);
void decreaseNearbyFieldsFlags(int currentX, int currentY);
void changeNearbyFieldsFlags(int currentX, int currentY, int value);
void changeFieldValue(int x, int y, int value);

void changeClickedFieldState(ALLEGRO_EVENT *event);

int getFieldSize();
std::vector<Map> getAllMaps();


#endif //MINESWEEPER_MAP_H
