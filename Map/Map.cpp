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
#include "../Menu/Menu.h"

using namespace std;

Map *map;
int fieldSize;
int fieldWithSpanSize;
int startX;
int startY;
int usedFlags = 0;
int score = 0;
bool isGame = false;
ALLEGRO_FONT *fieldsFont = nullptr;
ALLEGRO_FONT *statsFont = nullptr;

// private
int getCord(fstream *file) ;
void drawField(int startX, int startY, int x, int y, Field *field, int fieldWithSpanSize, bool showFlags);
void drawPlayer(Player *player, int startX, int startY, int fieldWithSpanSize);
ALLEGRO_COLOR getFieldColor(Field *field);

void initializeMapProperties();
void destroyMapProperties();

void drawStats(bool isGame) ;
void updateGame(int x, int y);

void initializeBombsIfNeed(int clickX, int clickY) ;

void changeNearbyFields(int currentX, int currentY, int value, void(*f)(int, int, int)) ;

// implementation
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
            for (int y = 0; y < map->sizeY; y++) {
                file << map->fields[x][y].type << " "
                     << map->fields[x][y].bombsInArea << endl;
            }
        }
        file.close();
    }
    al_shutdown_native_dialog_addon();
    return true;
}


void resetScore() {
    score = 0;
}
bool selectMap(bool isCustom, ALLEGRO_DISPLAY *display) {
    if (!isCustom) {
        return loadMap(display, getSelectedOptionName());
    } else {
        return loadMap(display, "");
    }
}

bool loadMap(ALLEGRO_DISPLAY * display, string path) {
    if (!al_init_native_dialog_addon()) return NULL;

    if (path.compare("") == 0) {
        ALLEGRO_FILECHOOSER *filechooser =
                al_create_native_file_dialog(MAPS_PATH.c_str(), "Load map.", "*.map", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
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
    initializeMapProperties();
    createFields();
    for (int x = 0; x < map->sizeX; x++) {
        for (int y = 0; y < map->sizeY; y++) {
            map->fields[x][y].wasVisited = false;
            map->fields[x][y].isFlagged = false;
            map->fields[x][y].type = (FieldType)getCord(&file);
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

void initializeMapProperties() {
    fieldSize = getFieldSize();
    usedFlags = 0;
    fieldWithSpanSize = (int)(fieldSize / (1 - FIELD_SPAN_RATIO));
    startX = (int)((SCREEN_WIDTH - (fieldWithSpanSize * map ->sizeX)) / 2.0);
    startY = (int)((SCREEN_HEIGHT - (fieldWithSpanSize* map ->sizeY)) / 2.0);
//    const char *fontPath = (FONT_PATH + MAIN_FONT).c_str();
    fieldsFont = al_load_font((FONT_PATH + MAIN_FONT).c_str(), (int)(fieldSize * 0.8), 0);
    statsFont = al_load_font(((FONT_PATH + MAIN_FONT).c_str()), (int)(startY * 0.8), 0);
}

void destroyMapProperties() {
    if (fieldsFont != nullptr) {
        al_destroy_font(fieldsFont);
    }
    if (statsFont != nullptr) {
        al_destroy_font(statsFont);
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
                delete [] map->fields[x];
            }
            delete [] map->fields;
        }
        delete map;
    }
    destroyMapProperties();
}

void displayMap(bool isGame) {
    for (int x = 0; x < map ->sizeX; x++) {
        for (int y = 0; y < map->sizeY; y++) {
            Field &field = map->fields[x][y];
            drawField(startX, startY, x, y, &field, fieldWithSpanSize, isGame);
        }
    }
    drawStats(isGame);
}




void drawField(int startX, int startY, int x, int y, Field *field, int fieldWithSpanSize, bool showFlags) {
    int emptyPlace = max((int)(fieldWithSpanSize * (FIELD_SPAN_RATIO) / 2.0), 1);
    ALLEGRO_COLOR color = getFieldColor(field);
    const int leftUpperAngleX = startX + fieldWithSpanSize * x +emptyPlace;
    const int leftUpperAngleY = startY + fieldWithSpanSize * y + emptyPlace;
    const int rightBottomAngleX = startX + fieldWithSpanSize * (x + 1) - emptyPlace;
    const int rightBottomAngleY = startY + fieldWithSpanSize * (y + 1) - emptyPlace;
    al_draw_filled_rectangle(
            leftUpperAngleX, leftUpperAngleY,
            rightBottomAngleX, rightBottomAngleY,
            color);
    
    if (showFlags && field->wasVisited && field->bombsInArea > 0 && field->type == FIELD_NORMAL) {
        al_draw_textf(fieldsFont, getColor(COLOR_HIGHLIGHT),
                      leftUpperAngleX + fieldSize / 2, leftUpperAngleY,
                      ALLEGRO_ALIGN_CENTER, "%i", field->bombsInArea);
    }
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

bool isGameFinished() {
    int foundBombs = 0;
    int checkedFields = 0;
    for (int x = 0; x < map ->sizeX; x++) {
        for (int y = 0; y < map->sizeX; y++) {
            Field &field = map->fields[x][y];
            if (field.wasVisited && field.type == FIELD_BOMB)
                return true;
            if (!field.wasVisited && field.isFlagged) {
                foundBombs++;
                checkedFields++;
            } else if (field.wasVisited) {
                checkedFields++;
            }
        }
    }
    return checkedFields >= map->sizeX * map->sizeY - map->emptyFields;
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
    if (map->fields[x][y].type == FIELD_BOMB) {
        map->fields[x][y].type = FIELD_NORMAL;
        decreaseNearbyFieldsFlags(x, y);
    }
}

void checkIfWasEmptyBefore(int x, int y) {
    if (map->fields[x][y].type == FIELD_EMPTY_SPACE) {
        map->fields[x][y].type = FIELD_NORMAL;
        map->emptyFields--;
    }
}

void checkIfWasFlaggedBefore(int x, int y) {
    if (map->fields[x][y].isFlagged) {
        usedFlags--;
    }
}

void cleanFieldState(int x, int y) {
    checkIfWasBombBefore(x, y);
    checkIfWasEmptyBefore(x, y);
    checkIfWasFlaggedBefore(x, y);
}

//void setFieldType(int x, int y, FieldType type){
//    map->fields[x][y].type = type;
//}
//
void setFieldAsBomb(int x, int y) {
    map->fields[x][y].type = FIELD_BOMB;
    increaseNearbyFieldsFlags(x, y);
}

void setFieldAsFlagged(int x, int y) {
    checkIfWasFlaggedBefore(x, y);
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
    changeNearbyFields(currentX, currentY, value, changeFieldValue);
}

void changeNearbyFields(int currentX, int currentY, int value, void(*f)(int, int, int)) {
    if (currentX > 0) {
        if (currentY > 0) {
            f(currentX - 1, currentY - 1, value);
        }
        f(currentX - 1, currentY, value);
        if (currentY + 1 < map->sizeY) {
            f(currentX - 1, currentY + 1, value);
        }
    }
    if (currentX + 1 < map->sizeX) {
        if (currentY > 0) {
            f(currentX + 1, currentY - 1, value);
        }
        f(currentX + 1, currentY, value);
        if (currentY + 1 < map->sizeY) {
            f(currentX + 1, currentY + 1, value);
        }
    }

    if (currentY > 0) {
        f(currentX, currentY - 1, value);
    }
    if (currentY + 1 < map->sizeY) {
        f(currentX, currentY + 1, value);
    }
}
void changeFieldValue(int x, int y, int value) {
    Field &field = map->fields[x][y];
    field.bombsInArea += value;
}

void showField(int x, int y, int value) {
    Field &field = map->fields[x][y];
    if (field.type == FIELD_NORMAL && !field.wasVisited) {
        field.wasVisited = true;
        score++;
        if (field.bombsInArea == 0) {
            changeNearbyFields(x, y, value, showField);
        }
    }
}

void showAllBombs(int clickX, int clickY) {
    for (int x = 0; x < map->sizeX; x++) {
        for (int y = 0; y < map->sizeY; y++) {
            if (map->fields[x][y].type == FIELD_BOMB) {
                map->fields[x][y].wasVisited = true;
            }
        }
    }
}

void maintainGame(ALLEGRO_EVENT *event){
    readUserOnMapClick(event, updateGame);
}

void maintainEditor(ALLEGRO_EVENT *event) {
    readUserOnMapClick(event, setNextEditorState);
}

void updateGame(int x, int y) {
    if (score == 0) {
        initializeBombsIfNeed(x, y);
    }
    Field &field = map->fields[x][y];
    if (!field.wasVisited) {
        if (field.type == FIELD_NORMAL) {
            showField(x, y, 0);
        } else if (field.type == FIELD_BOMB) {
            showAllBombs(x, y);
//            return false;
        }
    }
}


void initializeBombsIfNeed(int clickX, int clickY) {
    int currentBombsLimit = 0;
    const int bombsLimit = map->bombsLimit;
    for (int x = 0; x < map->sizeX; x++) {
        for (int y = 0; y < map->sizeY; y++) {
            if (map->fields[x][y].type == FIELD_BOMB) {
                increaseNearbyFieldsFlags(x, y);
                currentBombsLimit++;
            }
        }
    }

    srand((unsigned int) time(NULL));
    bool canContinue = true;
    while (currentBombsLimit < bombsLimit ) {
        for (int x = 0; x < map->sizeX && canContinue; x++) {
            for (int y = 0; y < map->sizeY && canContinue; y++) {
                if (map->fields[x][y].type == FIELD_NORMAL &&
                        !(x == clickX && y == clickY)) {
                    if (rand() % 100 > 95) {
                        currentBombsLimit++;
                        setFieldAsBomb(x, y);
                        if (currentBombsLimit >= bombsLimit) {
                            canContinue = false;
                        }
                    }
                }
            }
        }
    }
}


void drawStats(bool isGame) {
    const int textVerticalPosition = (int)(min(startY, startX) * 0.06);
    if (isGame) {
        al_draw_textf(statsFont, getColor(COLOR_NORMAL), SCREEN_WIDTH - 10, textVerticalPosition,
                      ALLEGRO_ALIGN_RIGHT, "Available flags : %i", map->bombsLimit - usedFlags);
    } else {
        al_draw_textf(statsFont, getColor(COLOR_BOMB), SCREEN_WIDTH - 10, textVerticalPosition,
                      ALLEGRO_ALIGN_RIGHT, "Bombs set : %i", map->bombsLimit);
    }
}

void readUserOnMapClick(ALLEGRO_EVENT *event, void(*f)(int, int)) {
    int mouseX = event->mouse.x;
    int mouseY = event->mouse.y;
    if (mouseX < startX || mouseY < startY) return;
    int x = (int)((mouseX - startX) / (double)fieldWithSpanSize);
    int y = (int)((mouseY - startY) / (double)fieldWithSpanSize);
    if (x < map->sizeX && y < map->sizeY && x >= 0 && y >= 0) {
        f(x, y);
    }
}

void setNextEditorState(int x, int y) {
    Field &field = map->fields[x][y];
    if (field.type == FIELD_NORMAL) {
        field.type = FIELD_BOMB;
        map->bombsLimit++;
    } else if (field.type == FIELD_BOMB) {
        field.type = FIELD_EMPTY_SPACE;
        map->bombsLimit--;
        map->emptyFields++;
    } else if (field.type == FIELD_EMPTY_SPACE) {
        field.type = FIELD_NORMAL;
        map->emptyFields--;
    }
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