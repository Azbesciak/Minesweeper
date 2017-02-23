#include <iostream>
#include "Player.h"

Player createPlayer(string name) {
    Player player;
    player.name = name;
    player.score = 0;
    player.x = 0;
}
Player loadPlayer(string name) {

}
vector<Player> getPlayers();
void presistPlayer(Player &player);
