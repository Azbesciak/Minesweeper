//
// Created by witek on 21.02.17.
//
#include <iostream>
#include <fstream>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include "Map.h"
#include <algorithm>
#include "../Menu/Menu.h"
#include "../../Game/Logic/GameLogic.h"

using namespace std;

// public
Map *map;
int fieldSize;
int fieldWithSpanSize;
int startX;
int startY;

// private
int emptySpace;
ALLEGRO_FONT *fieldsFont = nullptr;

ALLEGRO_COLOR getFieldColor(Field *field);

ALLEGRO_COLOR getBombsInfoColor(int bombsInArea);

ALLEGRO_COLOR getColor(int counter, bool isWin);

void initializeMapProperties();

void destroyMapProperties();

// implementation
bool persistMap(ALLEGRO_DISPLAY *display) {
    prepareMapToPersist();
    string target = MAPS_PATH + "new.map";
    if (!al_init_native_dialog_addon()) return false;
    ALLEGRO_FILECHOOSER *filechooser =
            al_create_native_file_dialog(target.c_str(), "Save map.", "*.map", ALLEGRO_FILECHOOSER_SAVE);
    al_show_native_file_dialog(display, filechooser);

    if (al_get_native_file_dialog_count(filechooser) > 0) {
        fstream file;
        file.open(al_get_native_file_dialog_path(filechooser, 0), ios::out);
        if (!file.good()) return false;

        file << map->sizeX << " " << map->sizeY << " "
             << map->bombsLimit << " " << map->emptyFields << endl;
        for (int x = 0; x < map->sizeX; x++) {
            for (int y = 0; y < map->sizeY; y++) {
                file << map->fields[x][y].type << " ";
            }
        }
        file.close();
    }
    al_shutdown_native_dialog_addon();
    return true;
}

bool selectMap(bool isCustom, ALLEGRO_DISPLAY *display) {
    if (!isCustom) {
        return loadMap(display, getSelectedOptionName());
    } else {
        return loadMap(display, "");
    }
}

bool loadMap(ALLEGRO_DISPLAY *display, string path) {
    if (!al_init_native_dialog_addon()) return false;

    if (path.compare("") == 0) {
        ALLEGRO_FILECHOOSER *filechooser =
                al_create_native_file_dialog(MAPS_PATH.c_str(), "Load map.", "*.map",
                                             ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
        if (!al_show_native_file_dialog(display, filechooser))
            return false;
        if (al_get_native_file_dialog_count(filechooser) > 0)
            path = al_get_native_file_dialog_path(filechooser, 0);
        else return false;
    } else {
        path = MAPS_PATH + path + ".map";
    }

    fstream file;
    file.open(path, ios::in);
    if (!file.good()) return false;
    destroyMap();
    map = new Map();
    map->sizeX = getIntFromFile(&file, ' ');
    map->sizeY = getIntFromFile(&file, ' ');
    map->bombsLimit = getIntFromFile(&file, ' ');
    map->emptyFields = getIntFromFile(&file);
    if (map->sizeX <= 0 || map->sizeX > 100 ||
        map->sizeY <= 0 || map->sizeY > 100 ||
        map->bombsLimit > map->sizeX * map->sizeY ||
        map->bombsLimit < 0) {
        delete map;
        return false;
    }
    initializeMapProperties();
    createFields();
    for (int x = 0; x < map->sizeX; x++) {
        for (int y = 0; y < map->sizeY; y++) {
            Field &field = map->fields[x][y];
            field.wasVisited = false;
            field.isFlagged = false;
            field.type = (FieldType) getIntFromFile(&file, ' ');
            field.bombsInArea = 0;
        }
    }
    file.close();
    return true;
}

void createFields() {
    map->fields = new Field *[map->sizeX];
    for (int x = 0; x < map->sizeX; x++) {
        map->fields[x] = new Field[map->sizeY];
    }
}

void initializeMapProperties() {
    fieldSize = getFieldSize();
    fieldWithSpanSize = (int) (fieldSize / (1 - FIELD_SPAN_RATIO));
    startX = (int) ((SCREEN_WIDTH - (fieldWithSpanSize * map->sizeX)) / 2.0);
    startY = (int) ((SCREEN_HEIGHT - (fieldWithSpanSize * map->sizeY)) / 2.0);
    fieldsFont = al_load_font((FONT_PATH + MAIN_FONT).c_str(), (int) (fieldSize * 0.8), 0);
    emptySpace = max((int) (fieldWithSpanSize * (FIELD_SPAN_RATIO) / 2.0), 1);
}

void destroyMapProperties() {
    if (fieldsFont != nullptr) {
        al_destroy_font(fieldsFont);
    }
}

void initializeEmptyMap(int sizeX, int sizeY) {
    destroyMap();
    map = new Map();
    map->sizeX = sizeX;
    map->sizeY = sizeY;
    initializeMapProperties();
    createFields();
    for (int x = 0; x < sizeX; x++) {
        for (int y = 0; y < sizeY; y++) {
            map->fields[x][y].wasVisited = true;
            map->fields[x][y].bombsInArea = 0;
            map->fields[x][y].isFlagged = false;
            map->fields[x][y].type = FIELD_NORMAL;
        }
    }
}

void destroyMap() {
    if (map != nullptr) {
        if (map->fields != nullptr) {
            for (int x = 0; x < map->sizeX; x++) {
                delete[] map->fields[x];
            }
            delete[] map->fields;
        }
        delete map;
    }
    destroyMapProperties();
}


void displayMap(bool isGame) {
    for (int x = 0; x < map->sizeX; x++) {
        for (int y = 0; y < map->sizeY; y++) {
            Field &field = map->fields[x][y];
            drawField(x, y, &field, fieldWithSpanSize, isGame);
        }
    }
    if (gameState == GAME_STATE_PLAYING) {
        drawStats(isGame);
    } else {
        drawGameEnd();
    }
}

void drawField(int x, int y, Field *field, int fieldWithSpanSize, bool showBombsInfo) {
    ALLEGRO_COLOR color = getFieldColor(field);
    const int leftUpperCornerX = startX + fieldWithSpanSize * x + emptySpace;
    const int leftUpperCornerY = startY + fieldWithSpanSize * y + emptySpace;
    drawFieldWithColor(x, y, color);

    if (showBombsInfo && field->wasVisited && field->bombsInArea > 0 && field->type == FIELD_NORMAL) {
        color = getBombsInfoColor(field->bombsInArea);

        al_draw_textf(fieldsFont, color,
                      leftUpperCornerX + fieldSize / 2, leftUpperCornerY,
                      ALLEGRO_ALIGN_CENTER, "%i", field->bombsInArea);
    }
}

ALLEGRO_COLOR getBombsInfoColor(int bombsInArea) {
    switch (bombsInArea) {
        case 1:
            return getColor(COLOR_1_BOMB);
        case 2:
            return getColor(COLOR_2_BOMB);
        case 3:
            return getColor(COLOR_3_BOMB);
        default:
            return getColor(COLOR_MORE_BOMB);
    }
}

void drawFieldWithColor(int x, int y, ALLEGRO_COLOR color) {
    const int leftUpperCornerX = startX + fieldWithSpanSize * x + emptySpace;
    const int leftUpperCornerY = startY + fieldWithSpanSize * y + emptySpace;
    const int rightBottomCornerX = startX + fieldWithSpanSize * (x + 1) - emptySpace;
    const int rightBottomCornerY = startY + fieldWithSpanSize * (y + 1) - emptySpace;
    al_draw_filled_rectangle(
            leftUpperCornerX, leftUpperCornerY,
            rightBottomCornerX, rightBottomCornerY,
            color);
}

void drawSmallFieldWithColor(int x, int y, ALLEGRO_COLOR color) {
    const int emptySpaceIncreased = emptySpace * 10;
    const int leftUpperCornerX = startX + fieldWithSpanSize * x + emptySpaceIncreased;
    const int leftUpperCornerY = startY + fieldWithSpanSize * y + emptySpaceIncreased;
    const int rightBottomCornerX = startX + fieldWithSpanSize * (x + 1) - emptySpaceIncreased;
    const int rightBottomCornerY = startY + fieldWithSpanSize * (y + 1) - emptySpaceIncreased;
    al_draw_filled_rectangle(
            leftUpperCornerX, leftUpperCornerY,
            rightBottomCornerX, rightBottomCornerY,
            color);
}


ALLEGRO_COLOR getFieldColor(Field *field) {
    if (field->type == FIELD_EMPTY_SPACE) {
        return getColor(COLOR_FREE_SPACE);
    } else if (field->wasVisited) {
        if (field->type == FIELD_BOMB) {
            return getColor(COLOR_BOMB);
        } else {
            return getColor(COLOR_VISITED);
        }
    } else {
        if (field->isFlagged) {
            return getColor(COLOR_FLAG);
        } else {
            return getColor(COLOR_UNKNOWN);
        }
    }
}

int getFieldSize() {
    int maxFieldAmount = max(map->sizeX, map->sizeY);
    double availablePlaceRatio = 1 - MARGIN_RATIO * 2;
    int maxSize = min((int) (SCREEN_HEIGHT * availablePlaceRatio), (int) (SCREEN_WIDTH * availablePlaceRatio));
    return (int) (maxSize / (double) maxFieldAmount * (1 - FIELD_SPAN_RATIO));
}


void prepareMapToPersist() {
    setBombsLimit(true);
}

void setBombsLimit(bool isSaveState) {
    int bombsLimitFromMenu = getBombsLimitFromMenu(isSaveState);
    if (bombsLimitFromMenu > map->bombsLimit) {
        map->bombsLimit = bombsLimitFromMenu;
    }
}

string getMapProps() {
    return to_string(map->sizeX) + "X" + to_string(map->sizeY) +
           "E" + to_string(map->emptyFields) +
           "B" + to_string(map->bombsLimit);
}
