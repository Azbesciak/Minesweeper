//
// Created by witek on 16.02.17.
//

#ifndef SAPPER_UTILS_H
#define SAPPER_UTILS_H

#include <allegro5/color.h>


enum Key{UP, DOWN, LEFT, RIGHT, SPACE, ENTER, ESC};
enum GameState{STATE_GAME, STATE_MAIN_MENU, STATE_EDITOR_MENU, STATE_EDITOR, STATE_SELECT_MODE, STATE_SAVE_MAP};

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int MAX_FIELDS = 100;
const int MIN_FIELDS = 5;
const double FPS = 24.0;


ALLEGRO_COLOR getMenuHighlightColor();
ALLEGRO_COLOR getMenuNormalColor();
ALLEGRO_COLOR getArrowHighlightColor();
ALLEGRO_COLOR getMenuBackgroundColor();
ALLEGRO_COLOR getGameBackgroundColor();
//Field createPoint(int, int);
#endif //SAPPER_UTILS_H
