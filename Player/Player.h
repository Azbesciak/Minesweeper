//
// Created by witek on 16.02.17.
//

#ifndef SAPPER_PLAYER_H
#define SAPPER_PLAYER_H
#include <iostream>
#include <vector>
#include "../Map/Map.h"

using namespace std;
struct Player {
    string name;
    int score;
    int x;
    int y;
};

Player createPlayer(string name);
Player loadPlayer(string name);
void setPlayerInitialPosition(Player *player, Map *map);
void movePlayer(Player *player, Map *map, int x, int y);
vector<Player> getPlayers();
void presistPlayer(Player &player);
#endif //SAPPER_PLAYER_H
