
#include <allegro5/color.h>
#include "Utils.h"

ColorsPalette *palette = nullptr;

void initializeColors() {
    if (palette == nullptr) {
        palette = new ColorsPalette();
        palette->color = new ALLEGRO_COLOR[COLORS_AMOUNT];
        setColor(COLOR_BACKGROUND, al_map_rgb(56, 142, 60));
        setColor(COLOR_NORMAL, al_map_rgb(139, 195, 74));
        setColor(COLOR_HIGHLIGHT, al_map_rgb(205, 220, 57));
        setColor(COLOR_HIGHLIGHT_PLUS, al_map_rgb(245, 124, 0));
        setColor(COLOR_FLAG, al_map_rgb(249, 168, 37));
        setColor(COLOR_BOMB, al_map_rgb(244, 67, 54));
        setColor(COLOR_FREE_SPACE, al_map_rgb(56, 142, 60));
        setColor(COLOR_VISITED, al_map_rgb(139, 195, 74));
        setColor(COLOR_UNKNOWN, al_map_rgb(142, 36, 170));
        setColor(COLOR_LOSE_SOURCE, al_map_rgb(0, 0, 0));
        setColor(COLOR_1_BOMB, al_map_rgb(0, 188, 212));
        setColor(COLOR_2_BOMB, al_map_rgb(0, 150, 136));
        setColor(COLOR_3_BOMB, al_map_rgb(103, 58, 183));
        setColor(COLOR_MORE_BOMB, al_map_rgb(233, 30, 99));
    }
}
void setColor(ColorId id, ALLEGRO_COLOR color) {
    palette->color[id] = color;
}
ALLEGRO_COLOR getColor(ColorId id) {
    return palette->color[id];
}

ALLEGRO_COLOR getMenuNormalColor() {
    return getColor(COLOR_NORMAL);
}
ALLEGRO_COLOR getMenuHighlightColor() {
    return getColor(COLOR_HIGHLIGHT);
}

ALLEGRO_COLOR getArrowHighlightColor() {
    return getColor(COLOR_HIGHLIGHT_PLUS);
}

ALLEGRO_COLOR getBackgroundColor() {
    return getColor(COLOR_BACKGROUND);
}

void destroyColors() {
    if (palette != nullptr) {
        if (palette->color != nullptr) {
            delete [] palette->color;
        }
        delete palette;
    }
}

void initializePoint(Point *point, int x, int y) {
    point = new Point();
    point->x = x;
    point->y = y;
}
void destroyPoint(Point *point) {
    if (point != nullptr) {
        delete point;
    }
}
int getX(Point *point) {
    if (point != nullptr) {
        return point->x;
    } else {
        return 0;
    }
}
void setX(Point *point, int x) {
    if (point != nullptr) {
        point->x = x;
    }
}
int getY(Point *point) {
    if (point != nullptr) {
        return point->y;
    } else {
        return 0;
    }
}
void setY(Point *point, int y);