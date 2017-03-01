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
int emptySpace;
GameState gameState;
Point *lastClick;
int usedFlags = 0;
int score = 0;
int timer = 0;

ALLEGRO_FONT *fieldsFont = nullptr;
ALLEGRO_FONT *statsFont = nullptr;
ALLEGRO_FONT *endGameFont = nullptr;

// private
int getCord(fstream *file);
int getCord(fstream *file, bool endLine);
void drawField(int x, int y, Field *field, int fieldWithSpanSize, bool showBombsInfo);
void drawPlayer(Player *player, int startX, int startY, int fieldWithSpanSize);
ALLEGRO_COLOR getFieldColor(Field *field);
ALLEGRO_COLOR getBombsInfoColor(int bombsInArea);

void initializeMapProperties();
void destroyMapProperties();
void drawStats(bool isGame);
void updateGame(int x, int y, ALLEGRO_EVENT * event);
void initializeBombsIfNeed(int clickX, int clickY);
void changeNearbyFields(int currentX, int currentY, int value, void(*f)(int, int, int)) ;
void initializeLastClick();
void destroyLastClick();
void setLastClickedField(int x, int y);
bool compareLastClickCords(int x, int y);
void setNextEditorState(int x, int y, ALLEGRO_EVENT *event);

void drawFieldWithColor(int x, int y, ALLEGRO_COLOR color) ;

void drawGameEnd() ;

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
                file << map->fields[x][y].type << " ";
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
    map->emptyFields = getCord(&file, true);
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
            field.type = (FieldType)getCord(&file);
            field.bombsInArea = 0;
        }
    }
    usedFlags = 0;
    return true;
}

int getCord(fstream *file) {
    return getCord(file, false);
}

int getCord(fstream *file, bool endLine) {
    string temp;
    if (endLine) {
        getline(*file, temp);
    } else {
        getline(*file, temp, ' ');
    }
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
    fieldsFont = al_load_font((FONT_PATH + MAIN_FONT).c_str(), (int)(fieldSize * 0.8), 0);
    emptySpace = max((int)(fieldWithSpanSize * (FIELD_SPAN_RATIO) / 2.0), 1);
    statsFont = al_load_font(((FONT_PATH + MAIN_FONT).c_str()), 36, 0);
    endGameFont = al_load_font(((FONT_PATH + MAIN_FONT).c_str()), (int)(SCREEN_HEIGHT * 0.2), 0);
    gameState = GAME_STATE_PLAYING;
    initializeLastClick();
}

void destroyMapProperties() {
    if (fieldsFont != nullptr) {
        al_destroy_font(fieldsFont);
    }
    if (statsFont != nullptr) {
        al_destroy_font(statsFont);
    }
    if (endGameFont != nullptr) {
        al_destroy_font(endGameFont);
    }

    destroyLastClick();
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

void displayGameResult() {
    al_draw_filled_rectangle(0, (int)(SCREEN_HEIGHT * 0.3), SCREEN_WIDTH, (int)(SCREEN_HEIGHT * 0.7), al_map_rgba(0, 0, 0, 100));
    string message;
    if (gameState == GAME_STATE_WON) {
        message = "You won";
    } else {
        message = "Game over";
    }
    al_draw_text(endGameFont, al_map_rgb(255, 255, 255), (int)(SCREEN_WIDTH / 2.0), (int)(SCREEN_HEIGHT * 0.3), ALLEGRO_ALIGN_CENTER, message.c_str());
    al_draw_textf(statsFont, al_map_rgb(255, 255, 255), (int)(SCREEN_WIDTH / 2.0), (int)(SCREEN_HEIGHT * 0.55), ALLEGRO_ALIGN_CENTER, "score : %i", score);
    al_draw_text(statsFont, al_map_rgb(120, 144, 156), (int)(SCREEN_WIDTH / 2.0), (int)(SCREEN_HEIGHT * 0.64), ALLEGRO_ALIGN_CENTER, "press enter to continue");
}

void displayMap(bool isGame) {
    for (int x = 0; x < map ->sizeX; x++) {
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
        case 1: return getColor(COLOR_1_BOMB);
        case 2: return getColor(COLOR_2_BOMB);
        case 3: return getColor(COLOR_3_BOMB);
        default: return getColor(COLOR_MORE_BOMB);
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
    int maxSize = min((int)(SCREEN_HEIGHT * availablePlaceRatio), (int)(SCREEN_WIDTH * availablePlaceRatio));
    return (int)(maxSize / (double) maxFieldAmount * (1 - FIELD_SPAN_RATIO));
}

void drawGameEnd() {
    for (int x = 0; x < map->sizeX; x++) {
        for (int y = 0; y < map->sizeY; y++) {
            Field &field = map->fields[x][y];
            if (field.type == FIELD_BOMB) {
                if (compareLastClickCords(x, y)) {
                    drawFieldWithColor(x, y, getColor(COLOR_LOSE_SOURCE));
                }
                    drawSmallFieldWithColor(x, y, getColor(COLOR_BOMB));
            }
        }
    }
}

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
    if (!field.wasVisited && !field.isFlagged) {
        if (field.type == FIELD_NORMAL || value == 1) {
            field.wasVisited = true;
        }
        if (value == 1) {
            value = 0;
        }
        if (field.type == FIELD_NORMAL) {
            score++;
            if (field.bombsInArea == 0) {
                changeNearbyFields(x, y, value, showField);
            }
        }

    }
}

void showAllBombs() {
    for (int x = 0; x < map->sizeX; x++) {
        for (int y = 0; y < map->sizeY; y++) {
            if (map->fields[x][y].type == FIELD_BOMB) {
                map->fields[x][y].wasVisited = true;
            }
        }
    }
}

bool maintainGame(ALLEGRO_EVENT *event){
    if (gameState == GAME_STATE_PLAYING) {
        readUserOnMapClick(event, updateGame);
    }
    return gameState == GAME_STATE_PLAYING;
}

void maintainEditor(ALLEGRO_EVENT *event) {
    readUserOnMapClick(event, setNextEditorState);
}

void updateGame(int x, int y, ALLEGRO_EVENT *event) {
    if (gameState == GAME_STATE_PLAYING) {
        Field &field = map->fields[x][y];
        if (field.type == FIELD_EMPTY_SPACE) return;

        if (event->mouse.button & 2) {
            if (field.isFlagged) {
                usedFlags--;
                field.isFlagged = false;
            } else if (!field.isFlagged && map->bombsLimit > usedFlags) {
                field.isFlagged = true;
                usedFlags++;
            }
        } else if (event->mouse.button & 1) {
            if (score == 0) {
                initializeBombsIfNeed(x, y);
            }
            if (!field.wasVisited) {
                if (field.isFlagged) {
                    field.isFlagged = false;
                    usedFlags--;
                }
                    showField(x, y, 1);
            }
        }

        setLastClickedField(x, y);
        isGameFinished();
    }
}


void isGameFinished() {
    int foundBombs = 0;
    int checkedFields = 0;
    for (int x = 0; x < map ->sizeX; x++) {
        for (int y = 0; y < map->sizeX; y++) {
            Field &field = map->fields[x][y];
            if (field.wasVisited && field.type == FIELD_BOMB) {
                gameState = GAME_STATE_LOST;
                cout<<"lost"<<endl;
                return;
            }
            if (!field.wasVisited && field.isFlagged) {
                foundBombs++;
                checkedFields++;
            } else if (field.wasVisited) {
                checkedFields++;
            }
        }
    }

    if (checkedFields >= map->sizeX * map->sizeY - map->emptyFields) {
        gameState = GAME_STATE_WON;
        setLastClickedField(-1, -1);
        cout<<"won"<<endl;
    } else {
        gameState = GAME_STATE_PLAYING;
    }
}

void initializeLastClick() {
    lastClick = new Point();
    lastClick->x = -1;
    lastClick->y = -1;
}
void destroyLastClick() {
    if (lastClick != nullptr) {
        delete lastClick;
    }
}

void setLastClickedField(int x, int y) {
    lastClick->x = x;
    lastClick->y = y;
}

bool compareLastClickCords(int x, int y) {
    return lastClick->x == x && lastClick->y == y;
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
void resetTimer() {
    timer = 0;
}

void incrementTimer() {
    if (gameState == GAME_STATE_PLAYING) {
        timer++;
    }
}

void drawStats(bool isGame) {
    const int textVerticalPosition = (int)(min(startY, startX) * 0.06);
    if (isGame) {

        al_draw_textf(statsFont, getColor(COLOR_NORMAL), 10, textVerticalPosition,
                      ALLEGRO_ALIGN_LEFT, "Score : %i", score);
        al_draw_textf(statsFont, getColor(COLOR_HIGHLIGHT), SCREEN_WIDTH / 2, textVerticalPosition,
                      ALLEGRO_ALIGN_CENTER, "%i : %i", timer / (int)(FPS * 60), int(timer / FPS) % 60);
        al_draw_textf(statsFont, getColor(COLOR_FLAG), SCREEN_WIDTH - 10, textVerticalPosition,
                      ALLEGRO_ALIGN_RIGHT, "Flags : %i", map->bombsLimit - usedFlags);
    } else {
        al_draw_textf(statsFont, getColor(COLOR_BOMB), SCREEN_WIDTH - 10, textVerticalPosition,
                      ALLEGRO_ALIGN_RIGHT, "Bombs set : %i", map->bombsLimit);
    }
}

void readUserOnMapClick(ALLEGRO_EVENT *event, void(*f)(int, int, ALLEGRO_EVENT *)) {
    int mouseX = event->mouse.x;
    int mouseY = event->mouse.y;
    if (mouseX < startX || mouseY < startY) return;
    int x = (int)((mouseX - startX) / (double)fieldWithSpanSize);
    int y = (int)((mouseY - startY) / (double)fieldWithSpanSize);
    if (x < map->sizeX && y < map->sizeY && x >= 0 && y >= 0) {
        f(x, y, event);
    }
}

void setNextEditorState(int x, int y, ALLEGRO_EVENT *event) {
    Field &field = map->fields[x][y];
//    if (event->mouse.button == 1) {
//        if (field.type == FIELD_EMPTY_SPACE) {
//            map->emptyFields--;
//        }
//        if (field.type != FIELD_BOMB) {
//            field.type = FIELD_BOMB;
//            map->bombsLimit++;
//        } else {
//
//        }
//    }
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