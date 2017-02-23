//
// Created by witek on 16.02.17.
//

#ifndef SAPPER_PLAYER_H
#define SAPPER_PLAYER_H
#include <iostream>
#include <vector>

using namespace std;
struct Player {
    string name;
    int score;
    int x;
    int y;
};

Player createPlayer(string name);
Player loadPlayer(string name);
vector<Player> getPlayers();
void presistPlayer(Player &player);
#endif //SAPPER_PLAYER_H
