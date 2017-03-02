#ifndef SAPPER_MENU_H
#define SAPPER_MENU_H

#include <iostream>
#include <allegro5/allegro_font.h>

using namespace std;
enum MainMenuOption {
    MENU_START_GAME, MENU_EDITOR, MENU_RANKING, MENU_QUIT
};
enum EditorMenuOption {
    EDITOR_MENU_CHOOSE_HEIGHT, EDITOR_MENU_CHOOSE_WIDTH, EDITOR_MENU_START, EDITOR_MENU_QUIT
};
enum EditorMenuPauseOption {
    EDITOR_PAUSE_SAVE, EDITOR_PAUSE_CONTINUE, EDITOR_PAUSE_RESET, EDITOR_PAUSE_QUIT
};
enum LevelDifficultyOption {
    DIF_EASY, DIF_NORMAL, DIF_HARD, DIF_CUSTOM, DIF_QUIT
};
enum SelectMapOption {
    SELECT_10X10, SELECT_25X25, SELECT_50X50, SELECT_CUSTOM, SELECT_BACK
};
enum GamePauseOption {
    GAME_PAUSE_CONTINUE, GAME_PAUSE_QUIT
};
enum SaveMapOptions {
    SAVE_BOMBS_LIMIT, SAVE_SAVE, SAVE_QUIT
};
enum GameFinishedOption {
    GAME_FINISHED_SAVE, GAME_FINISHED_QUIT
};
enum HorizontalOption {
    LESS, MORE
};
const int MAIN_MENU_OPTIONS = 4;
const int EDITOR_MENU_OPTIONS = 4;
const int LEVEL_DIFFICULTY_OPTIONS = 5;
const int SAVE_MAP_OPTIONS = 3;
const int SELECT_MAP_OPTIONS = 5;
const int GAME_PAUSE_OPTIONS = 2;
const int GAME_FINISHED_OPTIONS = 2;
const int MENU_ITEM_HEIGHT = 50;
struct MenuOption {
    string title;
    int value;
    int minValue;
    int maxValue;
    bool isChoice;
};

struct Menu {
    MenuOption *option;
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

void createSelectMapMenu();

void createLevelDifficultyMenu();

void createSaveMapMenu();

void createGamePauseMenu();

void createGameFinishedMenu();

void displayGameFinishedMenu();

string getSelectedOptionName();

int getBombsLimitFromMenu(bool isSaveState);

void createMenu(int options);

void destroyMenu();

void setOption(int optionId, string title);

void setOption(int optionId, string title, int value);

void setVariableOption(int optionId, string title, int value, int min, int max);

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

void maintainEnterHorizontalOption();

int getSelectedOption();

#endif //SAPPER_MENU_H
