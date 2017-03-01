#ifndef SAPPER_UTILS_H
#define SAPPER_UTILS_H

#include <fstream>

#include <allegro5/color.h>
#include <string>
using namespace std;
enum State {
    STATE_SELECT_MAP, STATE_GAME, STATE_GAME_PAUSE, STATE_MAIN_MENU, STATE_EDITOR_MENU,
    STATE_EDITOR, STATE_EDITOR_PAUSE, STATE_SELECT_DIFFICULTY, STATE_SAVE_MAP, STATE_GAME_FINISHED,
    STATE_SAVE_STATS, STATE_SHOW_RANKING
};
enum GameState {
    GAME_STATE_PLAYING, GAME_STATE_WON, GAME_STATE_LOST
};
enum ColorId {
    COLOR_BACKGROUND,
    COLOR_NORMAL,
    COLOR_HIGHLIGHT,
    COLOR_HIGHLIGHT_PLUS,
    COLOR_BOMB,
    COLOR_FREE_SPACE,
    COLOR_FLAG,
    COLOR_UNKNOWN,
    COLOR_VISITED,
    COLOR_LOSE_SOURCE,
    COLOR_1_BOMB,
    COLOR_2_BOMB,
    COLOR_3_BOMB,
    COLOR_MORE_BOMB,
};
const int COLORS_AMOUNT = 14;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int MAX_FIELDS = 100;
const int MIN_FIELDS = 5;
const double FPS = 24.0;
const double MARGIN_RATIO = 0.05;
const std::string FONT_PATH = "../resources/fonts/";
const std::string MAIN_FONT = "pricedown bl.ttf";

struct Point {
    int x;
    int y;
};

struct ColorsPalette {
    ALLEGRO_COLOR *color;
};
//extern ColorsPalette *palette;

void initializeColors();
void destroyColors();
void setColor(ColorId id, ALLEGRO_COLOR color);
ALLEGRO_COLOR getColor(ColorId id);
int getIntFromFile(fstream *file, char separator);
int getIntFromFile(fstream *file);
string getStringFromFile(fstream *file, char separator);
string getStringFromFile(fstream *file);
int parseGameTimeToSec(int gameTime);
string parseTime(int time);
ALLEGRO_COLOR getMenuHighlightColor();

ALLEGRO_COLOR getMenuNormalColor();

ALLEGRO_COLOR getArrowHighlightColor();

ALLEGRO_COLOR getBackgroundColor();

ALLEGRO_COLOR getGameBackgroundColor();
//Field createPoint(int, int);
#endif //SAPPER_UTILS_H
