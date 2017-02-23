//
// Created by witek on 21.02.17.
//
#include <iostream>
#include <fstream>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include "Map.h"
#include <algorithm>
#include "../Player/Player.h"

using namespace std;

int getCord(fstream *file) ;
void drawField(int startX, int startY, int x, int y, FieldType fieldType, int fieldWithSpanSize, bool wasVisited,
               bool isFlagged);
void drawPlayer(Player *player, int startX, int startY, int fieldWithSpanSize) ;

bool persistMap(Map *map, ALLEGRO_DISPLAY *display) {
    string target = MAPS_PATH +"new.map";
    if (!al_init_native_dialog_addon()) return false;
    ALLEGRO_FILECHOOSER *filechooser =
            al_create_native_file_dialog(target.c_str(), "Save map.", "*.map", ALLEGRO_FILECHOOSER_SAVE);
    al_show_native_file_dialog(display, filechooser);

    if (al_get_native_file_dialog_count(filechooser) > 0) {
        fstream file;
        file.open(al_get_native_file_dialog_path(filechooser, 0), ios::out);
        if (!file.good()) return false;

        file << map->sizeX << " " << map->sizeY << " " << map->bombsLimit<< endl;
        for (int x = 0; x < map->sizeX; x++) {
            for (int y = 0; x < map->sizeY; y++) {
                file << map->fields[x][y].type << " "
                     << map->fields[x][y].bombsInArea << endl;
            }
        }
        file.close();
    }
    al_shutdown_native_dialog_addon();
    return true;
}


Map loadMap(ALLEGRO_DISPLAY * display, string path) {
    if (!al_init_native_dialog_addon()) return NULL;

    if (path.compare("") == 0) {
        ALLEGRO_FILECHOOSER *filechooser =
                al_create_native_file_dialog(MAPS_PATH.c_str(), "Load map.", "*.map", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
        if (!al_show_native_file_dialog(display, filechooser)) return NULL;
        if (!al_get_native_file_dialog_count(filechooser) > 0)
            path = al_get_native_file_dialog_path(filechooser, 0);
        else return NULL;
    }
    fstream file;
    file.open(path, ios::in);
    if (!file.good()) return NULL;
    Map map;

    map.sizeX = getCord(&file);
    map.sizeY = getCord(&file);
    map.bombsLimit = getCord(&file);
    if (map.sizeX <= 0 || map.sizeX > 100 ||
            map.sizeY <= 0 || map.sizeY > 100 ||
            map.bombsLimit > map.sizeX * map.sizeY ||
            map.bombsLimit < 0) {
        return NULL;
    }
    for (int x = 0; x < map.sizeX; x++) {
        for (int y = 0; y < map.sizeY; y++) {
            map.fields[x][y].wasVisited = false;
            map.fields[x][y].isFlagged = false;
            map.fields[x][y].type = (FieldType) getCord(&file);
            map.fields[x][y].bombsInArea = getCord(&file);
        }
    }
    return map;
}

int getCord(fstream *file) {
    string temp;
    getline(*file, temp, ' ');
    return atoi(temp);
}


Map initializeEmptyMap(int sizeX, int sizeY) {
    Map map;
    map.sizeX = sizeX;
    map.sizeY = sizeY;
    for (int x = 0; x < sizeX; x++) {
        for (int y = 0; y < sizeY; y++) {
            map.fields[x][y].wasVisited = true;
            map.fields[x][y].type = FIELD_EMPTY;
            map.fields[x][y].bombsInArea = 0;
        }
    }
}

void displayMap(Map *map, Player *player, int fieldSize) {
    int fieldWithSpanSize = (int)(fieldSize / (1 - FIELD_SPAN_RATIO));
    int startX = (int)((SCREEN_WIDTH - (fieldWithSpanSize * map ->sizeX)) / 2.0);
    int startY = (int)((SCREEN_HEIGHT - (fieldWithSpanSize* map ->sizeY)) / 2.0);
    for (int x = 0; x < map ->sizeX; x++) {
        for (int y = 0; y < map->sizeX; y++) {
            Field &field = map->fields[x][y];
            FieldType fieldType = field.wasVisited ? field.type : FIELD_UNKNOWN;
            drawField(startX, startY, x, y, fieldType, fieldWithSpanSize, field.wasVisited, field.isFlagged);
        }
    }
    drawPlayer(player, startX, startY, fieldWithSpanSize);
}

ALLEGRO_COLOR getFieldColor(FieldType fieldType, bool visited, bool isFlagged);

void drawField(int startX, int startY, int x, int y, FieldType fieldType, int fieldWithSpanSize, bool wasVisited,
               bool isFlagged) {
    int emptyPlace = (int)(fieldWithSpanSize * (FIELD_SPAN_RATIO) /2.0);
    ALLEGRO_COLOR color = getFieldColor(fieldType, wasVisited, isFlagged);
    al_draw_filled_rectangle(
            startX + fieldWithSpanSize * x + emptyPlace, startY + fieldWithSpanSize * y + emptyPlace,
            startX + fieldWithSpanSize * (x + 1) - emptyPlace, startY + fieldWithSpanSize * (y + 1) - emptyPlace,
            color);
}

void drawPlayer(Player *player, int startX, int startY, int fieldWithSpanSize) {
    int emptyPlace = (int)(fieldWithSpanSize * (FIELD_SPAN_RATIO) /2.0);
    ALLEGRO_COLOR color = al_map_rgb(57, 73, 171);
    al_draw_filled_rectangle(
            startX + fieldWithSpanSize * player->x + emptyPlace, startY + fieldWithSpanSize * player->y + emptyPlace,
            startX + fieldWithSpanSize * (player->x + 1) - emptyPlace, startY + fieldWithSpanSize * (player->y + 1) - emptyPlace,
            color);
    color = al_map_rgb(142, 36, 170);
    al_draw_filled_rectangle(
            startX + fieldWithSpanSize * player->x + emptyPlace * 2, startY + fieldWithSpanSize * player->y + emptyPlace * 2,
            startX + fieldWithSpanSize * (player->x + 1) - emptyPlace * 2, startY + fieldWithSpanSize * (player->y + 1) - emptyPlace * 2,
            color);

}

ALLEGRO_COLOR getFieldColor(FieldType fieldType, bool visited, bool isFlagged) {
    if (fieldType == FIELD_WATER) return al_map_rgb(3, 169, 244);
    else if (!visited && !isFlagged) return al_map_rgb(224, 224, 224);
    else if (isFlagged) return al_map_rgb(224, 224, 224);
    else {
        switch (fieldType) {
            case FIELD_EMPTY: return al_map_rgb(220, 231, 117);
            case FIELD_BOMB: return al_map_rgb(229, 57, 53);
            case FIELD_NEST: return getMenuBackgroundColor();
            default: return al_map_rgb(255,255,255);
        }
    }

}

bool updateGameState(Map *map) {
    int foundBombs = 0;
    int checkedFields = 0;
    for (int x = 0; x < map ->sizeX; x++) {
        for (int y = 0; y < map->sizeX; y++) {
            Field &field = map->fields[x][y];
            if (field.wasVisited && field.type == FIELD_BOMB)
                return false;
            if (!field.wasVisited && field.isFlagged) {
                foundBombs++;
                checkedFields++;
            } else if (field.wasVisited) {
                checkedFields++;
            }

        }
    }
}
int getFieldSize(Map *map) {
    int maxFieldAmount = max(map->sizeX, map->sizeY);
    int maxSize = max(SCREEN_HEIGHT, SCREEN_WIDTH);
    return (int)(maxSize / (double)maxFieldAmount * (1 - FIELD_SPAN_RATIO));
}
std::vector<Map> getAllMaps() {

}
