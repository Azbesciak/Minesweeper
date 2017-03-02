//
// Created by witek on 02.03.17.
//

#ifndef MINESWEEPER_GAMELOGIC_H
#define MINESWEEPER_GAMELOGIC_H

#include <allegro5/events.h>
#include <string>

using namespace std;

void readUserOnMapClick(ALLEGRO_EVENT *event, bool ignoreOutsideMap, void(*f)(int, int, ALLEGRO_EVENT *));

void drawStats(bool isGame);

void drawGameEnd();

void resetScore();

int getGameScore();

void resetTimer();

void incrementTimer();

int getGameTimeInSec();

bool maintainGame(ALLEGRO_EVENT *event);

void maintainEditor(ALLEGRO_EVENT *event);

void isGameFinished();

void setBombsLimit(bool isSaveState);

void setFieldAsBomb(int x, int y);

void displayGameResult();

void increaseNearbyFieldsFlags(int currentX, int currentY);

void decreaseNearbyFieldsFlags(int currentX, int currentY);

void changeNearbyFieldsFlags(int currentX, int currentY, int value);

void changeFieldValue(int x, int y, int value);

std::string getCurrentTime();

void initializeGameLogicProps();

void destroyGameLogicProps();

void changeClickedFieldState(ALLEGRO_EVENT *event);

#endif //MINESWEEPER_GAMELOGIC_H
