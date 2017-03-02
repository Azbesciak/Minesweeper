//
// Created by witek on 02.03.17.
//

#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include <string>
#include <iostream>
#include "../../Utils/Utils.h"
#include "../../Functional/Map/Map.h"
#include "GameLogic.h"

using namespace std;


// public
GameState gameState;
int usedFlags;
int score;
int timer;

// private
Point *lastClick;
ALLEGRO_FONT *endGameFont;
ALLEGRO_FONT *statsFont = nullptr;

void setLastClickedField(int x, int y);

ALLEGRO_COLOR getColor(int counter, bool isWin);

void initializeBombsIfNeed(int clickX, int clickY);

bool compareLastClickCords(int x, int y);

void updateGame(int x, int y, ALLEGRO_EVENT *event);

void setNextEditorState(int x, int y, ALLEGRO_EVENT *);

void changeNearbyFields(int currentX, int currentY, int value, void (*f)(int, int, int));

void initializeLastClick();

void destroyLastClick();

// implementation
void initializeGameLogicProps() {
    destroyGameLogicProps();
    statsFont = al_load_font(((FONT_PATH + MAIN_FONT).c_str()), 36, 0);
    endGameFont = al_load_font(((FONT_PATH + MAIN_FONT).c_str()), (int) (SCREEN_HEIGHT * 0.2), 0);
    gameState = GAME_STATE_PLAYING;
    usedFlags = 0;
    resetScore();
    resetTimer();
    initializeLastClick();
}

void destroyGameLogicProps() {
    if (statsFont != nullptr) {
        al_destroy_font(statsFont);
    }
    if (endGameFont != nullptr) {
        al_destroy_font(endGameFont);
    }
    destroyLastClick();
}

void displayGameResult() {
    static int counter;
    al_draw_filled_rectangle(0, (int) (SCREEN_HEIGHT * 0.3), SCREEN_WIDTH, (int) (SCREEN_HEIGHT * 0.7),
                             al_map_rgba(0, 0, 0, 100));

    string message;
    bool isWin = gameState == GAME_STATE_WON;
    if (isWin) {
        message = "You won";
    } else {
        message = "Game over";
    }
    ALLEGRO_COLOR gameResultColor = getColor(counter, isWin);
    counter = (counter + 1) % (int) FPS;

    string result = "score : " + to_string(score) + "  time: " + getCurrentTime();
    al_draw_text(endGameFont, gameResultColor, (int) (SCREEN_WIDTH / 2.0),
                 (int) (SCREEN_HEIGHT * 0.3), ALLEGRO_ALIGN_CENTER, message.c_str());

    al_draw_text(statsFont, al_map_rgb(255, 255, 255), (int) (SCREEN_WIDTH / 2.0),
                 (int) (SCREEN_HEIGHT * 0.55), ALLEGRO_ALIGN_CENTER, result.c_str());
}

ALLEGRO_COLOR getColor(int counter, bool isWin) {
    if (counter < FPS / 8 || counter >= FPS / 8 * 7) {
        return isWin ? al_map_rgb(139, 195, 74) : al_map_rgb(244, 67, 54);
    } else if (counter < FPS / 8 * 2 || counter >= FPS / 8 * 6) {
        return isWin ? al_map_rgb(205, 220, 57) : al_map_rgb(233, 30, 99);
    } else if (counter < FPS / 8 * 3 || counter >= FPS / 8 * 5) {
        return isWin ? al_map_rgb(255, 235, 59) : al_map_rgb(156, 39, 176);
    } else {
        return isWin ? al_map_rgb(255, 193, 7) : al_map_rgb(103, 58, 183);
    }
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

void setFieldAsBomb(int x, int y) {
    map->fields[x][y].type = FIELD_BOMB;
    increaseNearbyFieldsFlags(x, y);
}

void increaseNearbyFieldsFlags(int currentX, int currentY) {
    changeNearbyFieldsFlags(currentX, currentY, 1);
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

bool maintainGame(ALLEGRO_EVENT *event) {
    if (gameState == GAME_STATE_PLAYING) {
        readUserOnMapClick(event, false, updateGame);
    }
    return gameState == GAME_STATE_PLAYING;
}

void maintainEditor(ALLEGRO_EVENT *event) {
    readUserOnMapClick(event, true, setNextEditorState);
}

void updateGame(int x, int y, ALLEGRO_EVENT *event) {

    static bool rightMouseButton;
    static bool leftMouseButton;
    if (gameState == GAME_STATE_PLAYING) {
        if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            if (x > -1 && compareLastClickCords(x, y)) {
                Field &field = map->fields[x][y];
                if (field.type != FIELD_EMPTY_SPACE) {

                    if (rightMouseButton && !leftMouseButton) {
                        if (!field.wasVisited) {
                            if (field.isFlagged) {
                                usedFlags--;
                                field.isFlagged = false;
                            } else if (!field.isFlagged && map->bombsLimit > usedFlags) {
                                field.isFlagged = true;
                                usedFlags++;
                            }
                        }
                    } else if (leftMouseButton && !rightMouseButton) {
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
                }

            }
            if (event->mouse.button & 1) {
                leftMouseButton = false;
            } else if (event->mouse.button & 2) {
                rightMouseButton = false;
            }
        } else if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (event->mouse.button & 1) {
                leftMouseButton = true;
            } else if (event->mouse.button & 2) {
                rightMouseButton = true;
            }
            setLastClickedField(x, y);
        }
        isGameFinished();
    }
}

void isGameFinished() {
    int foundBombs = 0;
    int checkedFields = 0;
    for (int x = 0; x < map->sizeX; x++) {
        for (int y = 0; y < map->sizeX; y++) {
            Field &field = map->fields[x][y];
            if (field.wasVisited && field.type == FIELD_BOMB) {
                gameState = GAME_STATE_LOST;
                cout << "lost" << endl;
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
        cout << "won" << endl;
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
    while (currentBombsLimit < bombsLimit) {
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

void resetScore() {
    score = 0;
}

void incrementTimer() {
    if (gameState == GAME_STATE_PLAYING) {
        timer++;
    }
}

int getGameTimeInSec() {
    return (int) (timer / FPS);
}

void drawStats(bool isGame) {
    const int textVerticalPosition = (int) (min(startY, startX) * 0.06);
    if (isGame) {

        al_draw_textf(statsFont, getColor(COLOR_NORMAL), 10, textVerticalPosition,
                      ALLEGRO_ALIGN_LEFT, "Score : %i", score);
        al_draw_text(statsFont, getColor(COLOR_HIGHLIGHT), SCREEN_WIDTH / 2, textVerticalPosition,
                     ALLEGRO_ALIGN_CENTER, getCurrentTime().c_str());
        al_draw_textf(statsFont, getColor(COLOR_FLAG), SCREEN_WIDTH - 10, textVerticalPosition,
                      ALLEGRO_ALIGN_RIGHT, "Flags : %i", map->bombsLimit - usedFlags);
    } else {
        al_draw_textf(statsFont, getColor(COLOR_BOMB), SCREEN_WIDTH - 10, textVerticalPosition,
                      ALLEGRO_ALIGN_RIGHT, "Bombs set : %i", map->bombsLimit);
    }
}

string getCurrentTime() {
    return parseTime(getGameTimeInSec());
}

void readUserOnMapClick(ALLEGRO_EVENT *event, bool ignoreOutsideMap, void(*f)(int, int, ALLEGRO_EVENT *)) {
    int mouseX = event->mouse.x;
    int mouseY = event->mouse.y;
    if (mouseX < startX || mouseY < startY) return;
    int x = (int) ((mouseX - startX) / (double) fieldWithSpanSize);
    int y = (int) ((mouseY - startY) / (double) fieldWithSpanSize);
    if (x < map->sizeX && y < map->sizeY && x >= 0 && y >= 0) {
        f(x, y, event);
    } else if (!ignoreOutsideMap) {
        x = -1;
        y = -1;
        f(x, y, event);
    }
}

void setNextEditorState(int x, int y, ALLEGRO_EVENT *event) {
    Field &field = map->fields[x][y];
    if (event->mouse.button & 1) {
        if (field.type == FIELD_BOMB) {
            map->bombsLimit--;
            field.type = FIELD_NORMAL;
        } else {
            if (field.type == FIELD_EMPTY_SPACE) {
                map->emptyFields--;
            }
            map->bombsLimit++;
            field.type = FIELD_BOMB;
        }
    } else if (event->mouse.button & 2) {
        if (field.type == FIELD_EMPTY_SPACE) {
            map->emptyFields--;
            field.type = FIELD_NORMAL;
        } else {
            if (field.type == FIELD_BOMB) {
                map->bombsLimit--;
            }
            map->emptyFields++;
            field.type = FIELD_EMPTY_SPACE;
        }
    }
}


int getGameScore() {
    return score;
}