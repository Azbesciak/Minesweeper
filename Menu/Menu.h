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
enum EditorMenuPauseOption{EDITOR_PAUSE_SAVE, EDITOR_PAUSE_CONTINUE, EDITOR_PAUSE_RESET, EDITOR_PAUSE_QUIT};
enum HorizontalOption{LESS, MORE};
static const int MAIN_MENU_OPTIONS = 3;
static const int EDITOR_MENU_OPTIONS = 4;
static const int MENU_ITEM_HEIGHT = 50;
struct Menu {
    string *option;
    int *value;
    int selectedVertically;
    int selectedHorizontally;
    int options;
};

extern Menu *menu;
extern ALLEGRO_FONT *menuFont;
void initializeMenuFont();
void destroyMenuFont();

void createMainMenu();
void createMapEditorMenu();
void createMapEditorPauseMenu();
void createMenu(int options);
void destroyMenu();
void setOption(int optionId, string name);
void setOptionWithValues(int optionId, string name, int value);
void displayMenu();
void setLowerOption(int maxLength);
void setHigherOption(int maxLength);
void setMoreOption();
void setLessOption();
void selectVerticalOption(int optionId);
void selectHorizontalOption(int option);
void increaseHorizontalOption();
void decreaseHorizontalOption();
void modifyHorizontalOption(int value);
void maintainEnterInEditorMenu();
int getSelectedOption();
#endif //SAPPER_MENU_H
