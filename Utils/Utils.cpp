
#include <allegro5/color.h>

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