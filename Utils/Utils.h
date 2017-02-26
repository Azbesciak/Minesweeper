#ifndef SAPPER_UTILS_H
#define SAPPER_UTILS_H

#include <allegro5/color.h>


enum Key {
    UP, DOWN, LEFT, RIGHT, SPACE, ENTER, ESC
};
enum GameState {
    STATE_SELECT_MAP, STATE_GAME, STATE_GAME_PAUSE, STATE_MAIN_MENU, STATE_EDITOR_MENU, STATE_EDITOR, STATE_EDITOR_PAUSE, STATE_SELECT_DIFFICULTY, STATE_SAVE_MAP
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
    COLOR_VISITED
};
const int COLORS_AMOUNT = 9;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int MAX_FIELDS = 100;
const int MIN_FIELDS = 5;
const double FPS = 24.0;
const double MARGIN_RATIO = 0.05;

struct Point {
    int x;
    int y;
};

struct ColorsPalette {
    ALLEGRO_COLOR *color;
};
extern ColorsPalette *palette;

void initializeColors();
void destroyColors();
void setColor(ColorId id, ALLEGRO_COLOR color);
ALLEGRO_COLOR getColor(ColorId id);

void initializePoint(Point *point, int x, int y);

void destroyPoint(Point *point);

int getX(Point *point);

void setX(Point *point, int x);

int getY(Point *point);

void setY(Point *point, int y);

ALLEGRO_COLOR getMenuHighlightColor();

ALLEGRO_COLOR getMenuNormalColor();

ALLEGRO_COLOR getArrowHighlightColor();

ALLEGRO_COLOR getBackgroundColor();

ALLEGRO_COLOR getGameBackgroundColor();
//Field createPoint(int, int);
#endif //SAPPER_UTILS_H
