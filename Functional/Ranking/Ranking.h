#ifndef MINESWEEPER_RANKING_H
#define MINESWEEPER_RANKING_H

#include <string>
#include <allegro5/allegro_font.h>

struct Stats {
    std::string playerName;
    int score;
    int time;
    std::string mapProps;
};
extern Stats *gameResult;
//extern ALLEGRO_FONT *statsFont;
const int MAX_STATS_ON_SCREEN = 10;
const std::string STATS_PATH = "../stats.txt";

bool saveStat();

void showStats();

bool initStats();

void destroyStats();

void getLowerStat();

void getHigherStat();

void createLastPlayerStats();

void destroyLastPlayerStats();

void updateLastPlayerName(ALLEGRO_EVENT *event);

void displayPlayerNameInput();

#endif //MINESWEEPER_RANKING_H
