//
// Created by witek on 20.02.17.
//

#ifndef SAPPER_MENU_H
#define SAPPER_MENU_H

#include <iostream>
#include <allegro5/allegro_font.h>
using namespace std;
enum MainMenuOption{MENU_START_GAME, MENU_EDITOR, MENU_QUIT};
enum EditorMenuOption{EDITOR_MENU_CHOOSE_HEIGHT, EDITOR_MENU_CHOOSE_WIDTH, EDITOR_MENU_START, EDITOR_MENU_QUIT};
enum HorizontalOption{LESS, MORE};
static const int MAIN_MENU_OPTIONS = 3;
static const int EDITOR_MENU_OPTIONS = 4;
static const int MENU_ITEM_HEIGHT = 50;
struct Menu {
    string option[10];
    int values[10];
    int selectedVertically;
    int selectedHorizontally;
};

Menu createMainMenu();
Menu createMapEditorMenu();
void displayMainMenu(Menu *menu, ALLEGRO_FONT *font);
void displayEditorMenu(Menu *menu, ALLEGRO_FONT *font);
void setLowerOption(Menu *menu, int maxLength);
void setHigherOption(Menu *menu, int maxLength);
void setMoreOption(Menu *menu);
void setLessOption(Menu *menu);
void setOption(Menu *menu, int optionId);
void increaseHorizontalOption(Menu *menu);
void decreaseHorizontalOption(Menu *menu);
void modifyHorizontalOption(Menu *menu, int value);
void maintainEnterInEditorMenu(Menu *menu);
int getSelectedOption(Menu *menu);
#endif //SAPPER_MENU_H
