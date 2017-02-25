//
// Created by witek on 21.02.17.
//
#include <iostream>
#include <fstream>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include "Map.h"
#include <algorithm>
#include "../Player/Player.h"

using namespace std;

Map *map;
int fieldSize;
int fieldWithSpanSize;
int startX;
int startY;
int usedFlags = 0;


int getCord(fstream *file) ;
void drawField(int startX, int startY, int x, int y, Field *field, int fieldWithSpanSize);
void drawPlayer(Player *player, int startX, int startY, int fieldWithSpanSize);

bool persistMap(ALLEGRO_DISPLAY *display) {
    prepareMapToPersist();
    string target = MAPS_PATH +"new.map";
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
            for (int y = 0; x < map->sizeY; y++) {
                file << map->fields[x][y].isEmptySpace << " "
                     << map->fields[x][y].isBomb << " "
                     << map->fields[x][y].bombsInArea << endl;
            }
        }
        file.close();
    }
    al_shutdown_native_dialog_addon();
    return true;
}


bool loadMap(ALLEGRO_DISPLAY * display, string path) {
    if (!al_init_native_dialog_addon()) return NULL;

    if (path.compare("") == 0) {
        ALLEGRO_FILECHOOSER *filechooser =
                al_create_native_file_dialog(MAPS_PATH.c_str(), "Load map.", "*.map", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
        if (!al_show_native_file_dialog(display, filechooser)) return false;
        if (!al_get_native_file_dialog_count(filechooser) > 0)
            path = al_get_native_file_dialog_path(filechooser, 0);
        else return false;
    }

    fstream file;
    file.open(path, ios::in);
    if (!file.good()) return false;
    map = new Map;
    map->sizeX = getCord(&file);
    map->sizeY = getCord(&file);
    map->bombsLimit = getCord(&file);
    map->emptyFields = getCord(&file);
    if (map->sizeX <= 0 || map->sizeX > 100 ||
            map->sizeY <= 0 || map->sizeY > 100 ||
            map->bombsLimit > map->sizeX * map->sizeY ||
            map->bombsLimit < 0) {
        delete map;
        return false;
    }
    for (int x = 0; x < map->sizeX; x++) {
        for (int y = 0; y < map->sizeY; y++) {
            map->fields[x][y].wasVisited = false;
            map->fields[x][y].isFlagged = false;
            map->fields[x][y].isEmptySpace = (bool)getCord(&file);
            map->fields[x][y].isBomb = (bool)getCord(&file);
            map->fields[x][y].bombsInArea = getCord(&file);
        }
    }
    usedFlags = 0;
    return true;
}

int getCord(fstream *file) {
    string temp;
    getline(*file, temp, ' ');
    return atoi(temp.c_str());
}

void createFields() {
    map ->fields = new Field*[map->sizeX] ;
    for (int x = 0; x < map ->sizeX; x++) {
        map ->fields[x] = new Field[map->sizeY];
    }
}

void initializeEmptyMap(int sizeX, int sizeY) {
    map = new Map();
    usedFlags = 0;
    map->sizeX = sizeX;
    map->sizeY = sizeY;
    fieldSize = getFieldSize();
    fieldWithSpanSize = (int)(fieldSize / (1 - FIELD_SPAN_RATIO));
    startX = (int)((SCREEN_WIDTH - (fieldWithSpanSize * map ->sizeX)) / 2.0);
    startY = (int)((SCREEN_HEIGHT - (fieldWithSpanSize* map ->sizeY)) / 2.0);
    createFields();
    for (int x = 0; x < sizeX; x++) {
        for (int y = 0; y < sizeY; y++) {
            map->fields[x][y].wasVisited = true;
            map->fields[x][y].isEmptySpace = false;
            map->fields[x][y].bombsInArea = 0;
            map->fields[x][y].isFlagged = false;
            map->fields[x][y].isBomb = false;
        }
    }
}

void destroyMap() {
    if (map != nullptr) {
        if (map->fields != nullptr) {
            for (int x = 0; x < map->sizeX; x++) {
                delete [] map->fields[x];
            }
            delete [] map->fields;
        }
        delete map;
    }
}

void displayMap() {

    for (int x = 0; x < map ->sizeX; x++) {
        for (int y = 0; y < map->sizeY; y++) {
            Field &field = map->fields[x][y];
            drawField(startX, startY, x, y, &field, fieldWithSpanSize);
        }
    }
}

ALLEGRO_COLOR getFieldColor(Field *field);

void drawField(int startX, int startY, int x, int y, Field *field, int fieldWithSpanSize) {
    int emptyPlace = max((int)(fieldWithSpanSize * (FIELD_SPAN_RATIO) / 2.0), 1);
    ALLEGRO_COLOR color = getFieldColor(field);
    al_draw_filled_rectangle(
            startX + fieldWithSpanSize * x + emptyPlace, startY + fieldWithSpanSize * y + emptyPlace,
            startX + fieldWithSpanSize * (x + 1) - emptyPlace, startY + fieldWithSpanSize * (y + 1) - emptyPlace,
            color);
}

//void drawPlayer(Player *player, int startX, int startY, int fieldWithSpanSize) {
//    int emptyPlace = (int)(fieldWithSpanSize * (FIELD_SPAN_RATIO) /2.0);
//    ALLEGRO_COLOR color = al_map_rgb(57, 73, 171);
//    al_draw_filled_rectangle(
//            startX + fieldWithSpanSize * player->x + emptyPlace, startY + fieldWithSpanSize * player->y + emptyPlace,
//            startX + fieldWithSpanSize * (player->x + 1) - emptyPlace, startY + fieldWithSpanSize * (player->y + 1) - emptyPlace,
//            color);
//    color = al_map_rgb(142, 36, 170);
//    al_draw_filled_rectangle(
//            startX + fieldWithSpanSize * player->x + emptyPlace * 2, startY + fieldWithSpanSize * player->y + emptyPlace * 2,
//            startX + fieldWithSpanSize * (player->x + 1) - emptyPlace * 2, startY + fieldWithSpanSize * (player->y + 1) - emptyPlace * 2,
//            color);
//
//}

ALLEGRO_COLOR getFieldColor(Field *field) {
    if (field->isEmptySpace) return getColor(COLOR_FREE_SPACE);
    else if (field->isBomb && field->wasVisited) return getColor(COLOR_BOMB);
    else if (!field->wasVisited && !field->isFlagged) return getColor(COLOR_NORMAL);
    else if (field->isFlagged) return getColor(COLOR_FLAG);
    else if (field->wasVisited) return getColor(COLOR_VISITED);
    else return getColor(COLOR_UNKNOWN);
}

bool isGameFinished() {
    int foundBombs = 0;
    int checkedFields = 0;
    for (int x = 0; x < map ->sizeX; x++) {
        for (int y = 0; y < map->sizeX; y++) {
            Field &field = map->fields[x][y];
            if (field.wasVisited && field.isBomb)
                return true;
            if (!field.wasVisited && field.isFlagged) {
                foundBombs++;
                checkedFields++;
            } else if (field.wasVisited) {
                checkedFields++;
            }
        }
        return checkedFields >= map->sizeX * map->sizeY - map->emptyFields;
    }
}


int getFieldSize() {
    int maxFieldAmount = max(map->sizeX, map->sizeY);
    double availablePlaceRatio = 1 - MARGIN_RATIO * 2;
    int maxSize = min((int)(SCREEN_HEIGHT * availablePlaceRatio), (int)(SCREEN_WIDTH * availablePlaceRatio));
    return (int)(maxSize / (double) maxFieldAmount * (1 - FIELD_SPAN_RATIO));
}
//std::vector<Map> getAllMaps() {
//
//}




void checkIfWasBombBefore(int x, int y) {
    if (map->fields[x][y].isBomb) {
        map->fields[x][y].isBomb = false;
        decreaseNearbyFieldsFlags(x, y);
    }
}

void checkIfWasEmptyBefore(int x, int y) {
    if (map->fields[x][y].isEmptySpace) {
        map->fields[x][y].isEmptySpace = false;
        map->emptyFields--;
    }
}

void checkIfWasFlagedBefore(int x, int y) {
    if (map->fields[x][y].isFlagged) {
        usedFlags--;
    }
}

void cleanFieldState(int x, int y) {
    checkIfWasBombBefore(x, y);
    checkIfWasEmptyBefore(x, y);
    checkIfWasFlagedBefore(x, y);
}

//void setFieldType(int x, int y, FieldType type){
//    map->fields[x][y].type = type;
//}
//
void setFieldAsBomb(int x, int y) {
    cleanFieldState(x, y);
    map->fields[x][y].isBomb = true;
    increaseNearbyFieldsFlags(x, y);
}

void setFieldAsEmpty(int x, int y) {
    cleanFieldState(x, y);
    map->fields[x][y].isEmptySpace = true;
    map->emptyFields++;
}

void setFieldAsUnknown(int x, int y) {
    cleanFieldState(x, y);
}
void setFieldAsFlagged(int x, int y) {
    checkIfWasFlagedBefore(x, y);
    map->fields[x][y].isFlagged = true;
    usedFlags++;
}
//void setFieldAsVirgin(int x, int y);
//
void increaseNearbyFieldsFlags(int currentX, int currentY) {
    changeNearbyFieldsFlags(currentX, currentY, 1);
}
void decreaseNearbyFieldsFlags(int currentX, int currentY) {
    changeNearbyFieldsFlags(currentX, currentY, -1);
}

void changeNearbyFieldsFlags(int currentX, int currentY, int value) {
    if (currentX > 0) {
        if (currentY > 0) {
            changeFieldValue(&map->fields[currentX - 1][currentY - 1], value);
        }
        changeFieldValue(&map->fields[currentX - 1][currentY], value);
        if (currentY + 1 < map->sizeY) {
            changeFieldValue(&map->fields[currentX - 1][currentY + 1], value);
        }
    }
    if (currentX + 1 < map->sizeX) {
        if (currentY > 0) {
            changeFieldValue(&map->fields[currentX + 1][currentY - 1], value);
        }
        changeFieldValue(&map->fields[currentX + 1][currentY], value);
        if (currentY + 1 < map->sizeY) {
            changeFieldValue(&map->fields[currentX + 1][currentY + 1], value);
        }
    }

    if (currentY > 0) {
        changeFieldValue(&map->fields[currentX][currentY - 1], value);
    }
    if (currentY + 1 < map->sizeY) {
        changeFieldValue(&map->fields[currentX][currentY + 1], value);
    }
}
void changeFieldValue(Field *field, int value) {
    field->bombsInArea += value;
}

void changeClickedFieldState(ALLEGRO_EVENT *event) {
    int mouseX = event->mouse.x;
    int mouseY = event->mouse.y;
    if (mouseX < startX || mouseY < startY) return;
    int x = (int)((mouseX - startX) / (double)fieldWithSpanSize);
    int y = (int)((mouseY - startY) / (double)fieldWithSpanSize);
    if (x < map->sizeX && y < map->sizeY && x >= 0 && y >= 0) {
        setNextEditorState(map, x, y);
    }
}
void setNextEditorState(Map *map, int x, int y) {

    Field &field = map->fields[x][y];
    if (field.isEmptySpace) {
        field.isEmptySpace = false;
    } else if (!field.isBomb) {
        field.isBomb = true;
    } else if (field.isBomb) {
        field.isEmptySpace = true;
        field.isBomb = false;
    }
}

void prepareMapToPersist() {
    for (int x = 0; x < map->sizeX; x++) {
        for (int y = 0; y< map->sizeY; y++) {
            if (map->fields[x][y].isEmptySpace) {
                setFieldAsEmpty(x, y);
            } else if (map->fields[x][y].isBomb){
                setFieldAsBomb(x, y);
            } else {
                setFieldAsUnknown(x, y);
            }
        }
    }
}




