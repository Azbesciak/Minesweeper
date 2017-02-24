
#include <allegro5/color.h>
#include "Utils.h"
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

ALLEGRO_COLOR getMenuNormalColor() {
    return al_map_rgb(139, 195, 74);
}
ALLEGRO_COLOR getMenuHighlightColor() {
    return al_map_rgb(205, 220, 57);
}

ALLEGRO_COLOR getArrowHighlightColor() {
    return al_map_rgb(245, 124, 0);
}

ALLEGRO_COLOR getMenuBackgroundColor() {
    return al_map_rgb(56, 142, 60);
}

ALLEGRO_COLOR getGameBackgroundColor() {
    return al_map_rgb(46, 132, 50);
}