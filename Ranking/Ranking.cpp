#include "Ranking.h"
#include <iostream>
#include <fstream>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "../Utils/Utils.h"
#include "../Menu/Menu.h"
#include "../Map/Map.h"

using namespace std;

Stats *ranking = nullptr;
Stats *gameResult = nullptr;
int rankingCursor;
int records;
int rowHeight;
//ALLEGRO_FONT *rankingFont;

//private
void sortStats(Stats *stats, int amount) ;
bool initStats(bool isAddMode);
void printStat(int index, int yPosition);
double countRank(Stats * stat);
//implementation
bool saveStat() {
    initStats(true);
    if (gameResult->playerName.compare("") == 0) {
        gameResult->playerName = "<unknown>";
    }
    ranking[records] = *gameResult;
    sortStats(ranking, records + 1);

    fstream file;
    file.open(STATS_PATH, ios::out);
    if (!file.good()) return false;
    file << records + 1 << endl;
    for (int i = 0; i < records + 1; i++) {
        file << ranking[i].playerName << ","
             << ranking[i].mapProps << ","
             << ranking[i].score << ","
             << ranking[i].time << endl;
    }

    destroyStats();
    return true;
}

bool initStats() {
    rankingCursor = 0;
    records = 0;
    return initStats(false);
}

bool initStats(bool isAddMode) {
    fstream file;
    file.open(STATS_PATH, ios::in);
    std::string temp;
    if (!file.good() && isAddMode) {
        records = 0;
    } else if (file.good()){
        getline(file, temp);
        records = atoi(temp.c_str());
    } else {
        return false;
    }
    ranking = new Stats[records + isAddMode]();
    rowHeight = (int)(SCREEN_HEIGHT / (double) MAX_STATS_ON_SCREEN * 0.6);
    for (int i = 0; i < records; i++) {
        ranking[i].playerName = getStringFromFile(&file, ',');
        ranking[i].mapProps = getStringFromFile(&file, ',');
        ranking[i].score = getIntFromFile(&file, ',');
        ranking[i].time = getIntFromFile(&file);
        ranking[i].rank = countRank(&ranking[i]);
    }
    file.close();
    return true;
}

void createLastPlayerStats() {
    gameResult = new Stats();
    gameResult->time = getGameTimeInSec();
    gameResult->playerName = "";
    gameResult->mapProps = getMapProps();
    gameResult->score = getGameScore();
    gameResult->rank = countRank(gameResult);
}

void destroyLastPlayerStats() {
    if (gameResult != nullptr) {
        delete gameResult;
    }
}

void updateLastPlayerName(ALLEGRO_EVENT * event) {
    if (gameResult != nullptr) {
        if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
            const int i = event->keyboard.keycode;
            if (i <= 26) {
                gameResult->playerName += (char)(i + 64);

            } else if (i == ALLEGRO_KEY_SPACE) {
                gameResult->playerName += " ";
            } else if (i == ALLEGRO_KEY_BACKSPACE && gameResult->playerName.length() > 0) {
                gameResult->playerName.pop_back();
            }

        }
    }
}

void displayPlayerNameInput() {
    unsigned long nameLength = gameResult->playerName.length() + 1;
    int yPosition = (int)(SCREEN_HEIGHT * 0.5);
    al_draw_text(menuFont, getColor(COLOR_HIGHLIGHT), SCREEN_WIDTH / 2, yPosition - 40, ALLEGRO_ALIGN_CENTER, gameResult->playerName.c_str());
    al_draw_line(SCREEN_WIDTH / 2 - nameLength * 10, yPosition,  SCREEN_WIDTH / 2 + nameLength * 10, yPosition, getColor(COLOR_HIGHLIGHT_PLUS), 3);

    al_draw_text(menuFont, getColor(COLOR_HIGHLIGHT), SCREEN_WIDTH / 2, SCREEN_HEIGHT - 90, ALLEGRO_ALIGN_CENTER, "Save");

    al_draw_text(menuFont, getColor(COLOR_NORMAL), SCREEN_WIDTH / 2, SCREEN_HEIGHT - 60, ALLEGRO_ALIGN_CENTER, "Press esc to go back");
}

void destroyStats() {
    if (ranking != nullptr) {
        delete [] ranking;
    }
}
void showStats() {
    int i = rankingCursor;
    int position = 0;
    if (records == 0) {
        al_draw_text(menuFont, getColor(COLOR_NORMAL), SCREEN_WIDTH / 2, (int)(SCREEN_HEIGHT * 0.45), ALLEGRO_ALIGN_CENTER, "Nothing out there");
    } else {
        while (i < min(rankingCursor + MAX_STATS_ON_SCREEN, records)) {
            int currentRowY = SCREEN_HEIGHT / 2  - (min(records, MAX_STATS_ON_SCREEN) / 2) * rowHeight + position;
            position += rowHeight;
            printStat(i, currentRowY);
            i++;
        }
    }
    al_draw_text(menuFont, getColor(COLOR_HIGHLIGHT), SCREEN_WIDTH / 2, SCREEN_HEIGHT - 60, ALLEGRO_ALIGN_CENTER, "Press esc to go back");
}

void printStat(int index, int yPosition) {
    Stats &stats = ranking[index];
    string info = to_string(index + 1) + ". " + stats.playerName + "    " +
            stats.mapProps + "    " + to_string(stats.score) + "    " +  parseTime(stats.time);
    al_draw_text(menuFont, getColor(COLOR_NORMAL), SCREEN_WIDTH / 2, yPosition, ALLEGRO_ALIGN_CENTER, info.c_str());
}

void sortStats(Stats *stats, int amount) {
    for (int i = 0; i < amount; i++) {
        for (int y = i; y < amount; y++) {
            if (stats[i].rank < stats[y].rank) {
                swap(stats[i], stats[y]);
            }
        }
    }
}

void getLowerStat() {
    if (rankingCursor - 1 >= 0) {
        rankingCursor--;
    }
}
void getHigherStat() {
    if (rankingCursor + 1 + MAX_STATS_ON_SCREEN <= records) {
        rankingCursor++;
    }
}

double countRank(Stats * stat) {
    if (stat->time == 0) {
        stat->time = 1;
    }
    return stat->score / (double)stat->time;
}

