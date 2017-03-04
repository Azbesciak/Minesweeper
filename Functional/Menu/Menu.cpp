#include "../../Utils/Utils.h"
#include "Menu.h"
#include "../Map/Map.h"
#include <allegro5/allegro_primitives.h>

// public
Menu *menu = nullptr;
ALLEGRO_FONT *menuFont = nullptr;

// implementation
void initializeMenuFont() {
    menuFont = al_load_font((FONT_PATH + MAIN_FONT).c_str(), 36, 0);
}

void destroyMenuFont() {
    al_destroy_font(menuFont);
}

void createMainMenu() {
    destroyMenu();
    createMenu(MAIN_MENU_OPTIONS);
    setOption(MENU_START_GAME, "Play game");
    setOption(MENU_EDITOR, "Editor");
    setOption(MENU_RANKING, "Ranking");
    setOption(MENU_QUIT, "Exit");
    selectVerticalOption(MENU_START_GAME);
}

void createMapEditorMenu() {
    destroyMenu();
    createMenu(EDITOR_MENU_OPTIONS);
    setVariableOption(EDITOR_MENU_CHOOSE_HEIGHT, "Height", 10, MIN_FIELDS, MAX_FIELDS);
    setVariableOption(EDITOR_MENU_CHOOSE_WIDTH, "Width", 10, MIN_FIELDS, MAX_FIELDS);
    setOption(EDITOR_MENU_START, "Create");
    setOption(EDITOR_MENU_QUIT, "Main menu");
    selectVerticalOption(EDITOR_MENU_CHOOSE_HEIGHT);
    selectHorizontalOption(MORE);
}

void createMapEditorPauseMenu() {
    destroyMenu();
    createMenu(EDITOR_MENU_OPTIONS);
    setOption(EDITOR_PAUSE_SAVE, "Save");
    setOption(EDITOR_PAUSE_CONTINUE, "Continue");
    setOption(EDITOR_PAUSE_RESET, "Reset");
    setOption(EDITOR_PAUSE_QUIT, "Main menu");
    selectVerticalOption(EDITOR_PAUSE_CONTINUE);
}

void createLevelDifficultyMenu() {
    destroyMenu();
    createMenu(LEVEL_DIFFICULTY_OPTIONS);
    setOption(DIF_EASY, "Easy (10%)", 10);
    setOption(DIF_NORMAL, "Normal (20%)", 20);
    setOption(DIF_HARD, "Hard (30%)", 30);
    setOption(DIF_CUSTOM, "Map difficulty");
    setOption(DIF_QUIT, "Back");
    selectVerticalOption(DIF_EASY);
    selectHorizontalOption(MORE);
}

void createSaveMapMenu() {
    destroyMenu();
    createMenu(SAVE_MAP_OPTIONS);
    int availableFields = map->sizeX * map->sizeY - map->emptyFields;
    int minPercentage = (int) (map->bombsLimit * 100 / (double) availableFields);
    int maxPercentage = 99;
    int currentValue = max(15, minPercentage);
    setVariableOption(SAVE_BOMBS_LIMIT, "Bombs limit (%)", currentValue, minPercentage, maxPercentage);
    setOption(SAVE_SAVE, "Save");
    setOption(SAVE_QUIT, "Back");
    selectVerticalOption(SAVE_BOMBS_LIMIT);
    selectHorizontalOption(MORE);
}

void createSelectMapMenu() {
    destroyMenu();
    createMenu(SELECT_MAP_OPTIONS);
    setOption(SELECT_10X10, "10X10");
    setOption(SELECT_25X25, "2525");
    setOption(SELECT_50X50, "50X50");
    setOption(SELECT_CUSTOM, "Custom");
    setOption(SELECT_BACK, "Back");
    selectVerticalOption(SELECT_10X10);
}

void createGamePauseMenu() {
    destroyMenu();
    createMenu(GAME_PAUSE_OPTIONS);
    setOption(GAME_PAUSE_CONTINUE, "Continue");
    setOption(GAME_PAUSE_QUIT, "Main menu");
    selectVerticalOption(GAME_PAUSE_CONTINUE);
}

void createGameFinishedMenu() {
    destroyMenu();
    createMenu(GAME_FINISHED_OPTIONS);
    setOption(GAME_FINISHED_SAVE, "Save result");
    setOption(GAME_FINISHED_QUIT, "Main menu");
    selectVerticalOption(GAME_FINISHED_SAVE);
}

void createMenu(int options) {
    menu = new Menu();
    menu->option = new MenuOption[options];
    menu->options = options;
}

void destroyMenu() {
    if (menu != nullptr) {
        if (menu->option != nullptr) {
            delete[] menu->option;
        }
        delete menu;
    }
}

void setOption(int optionId, string title) {
    MenuOption &option = menu->option[optionId];
    option.title = title;
    option.value = -1;
    option.isChoice = false;
}

void setOption(int optionId, string title, int value) {
    MenuOption &option = menu->option[optionId];
    option.title = title;
    option.value = value;
    option.isChoice = false;
}

void setVariableOption(int optionId, string title, int value, int min, int max) {
    MenuOption &option = menu->option[optionId];
    option.title = title;
    option.value = value;
    option.minValue = min;
    option.maxValue = max;
    option.isChoice = true;
}

void displayGameFinishedMenu() {
    ALLEGRO_COLOR HIGHLIGHT_COLOR = getMenuHighlightColor();
    ALLEGRO_COLOR NORMAL_COLOR = getMenuNormalColor();
    int lineX = SCREEN_WIDTH / 2;
    int lineY = (int) (SCREEN_HEIGHT * 0.64);

    ALLEGRO_COLOR colorSave = menu->selectedVertically == GAME_FINISHED_SAVE ? HIGHLIGHT_COLOR : NORMAL_COLOR;
    ALLEGRO_COLOR colorQuit = menu->selectedVertically == GAME_FINISHED_QUIT ? HIGHLIGHT_COLOR : NORMAL_COLOR;

    al_draw_text(menuFont, colorSave, lineX - 20, lineY, ALLEGRO_ALIGN_RIGHT,
                 menu->option[GAME_FINISHED_SAVE].title.c_str());
    al_draw_text(menuFont, colorQuit, lineX + 20, lineY, ALLEGRO_ALIGN_LEFT,
                 menu->option[GAME_FINISHED_QUIT].title.c_str());
}

void displayMenu() {
    ALLEGRO_COLOR HIGHLIGHT_COLOR = getMenuHighlightColor();
    ALLEGRO_COLOR NORMAL_COLOR = getMenuNormalColor();
    const int lineX = SCREEN_WIDTH / 2;
    for (int option = 0; option < menu->options; option++) {
        int lineY = SCREEN_HEIGHT / 2 + option * MENU_ITEM_HEIGHT - (int) (menu->options / 2.0 * MENU_ITEM_HEIGHT);
        ALLEGRO_COLOR color = menu->selectedVertically == option ? HIGHLIGHT_COLOR : NORMAL_COLOR;

        if (menu->option[option].isChoice) {
            al_draw_text(menuFont, color, lineX, lineY, ALLEGRO_ALIGN_RIGHT, menu->option[option].title.c_str());
            ALLEGRO_COLOR triangleColor =
                    menu->selectedVertically == option && menu->selectedHorizontally == LESS ? getArrowHighlightColor()
                                                                                             : color;
            al_draw_filled_triangle(SCREEN_WIDTH / 2 + 20, lineY + 25,
                                    SCREEN_WIDTH / 2 + 35, lineY + 15,
                                    SCREEN_WIDTH / 2 + 35, lineY + 35, triangleColor);

            al_draw_textf(menuFont, color, SCREEN_WIDTH / 2 + 60, lineY, ALLEGRO_ALIGN_CENTER, "%i",
                          menu->option[option].value);

            triangleColor =
                    menu->selectedVertically == option && menu->selectedHorizontally == MORE ? getArrowHighlightColor()
                                                                                             : color;
            al_draw_filled_triangle(SCREEN_WIDTH / 2 + 85, lineY + 15,
                                    SCREEN_WIDTH / 2 + 100, lineY + 25,
                                    SCREEN_WIDTH / 2 + 85, lineY + 35, triangleColor);
        } else {
            al_draw_text(menuFont, color, lineX, lineY, ALLEGRO_ALIGN_CENTER, menu->option[option].title.c_str());
        }
    }
}

void setLowerOption(int maxLength) {
    int nextOption = (menu->selectedVertically + 1) % maxLength;
    selectVerticalOption(nextOption);
}

void setHigherOption(int maxLength) {
    int nextOption = menu->selectedVertically - 1 < 0 ? maxLength - 1 : menu->selectedVertically - 1;
    selectVerticalOption(nextOption);
}

void selectVerticalOption(int optionId) {
    menu->selectedVertically = optionId;
}

int getSelectedOption() {
    return menu->selectedVertically;
}

void selectHorizontalOption(int option) {
    menu->selectedHorizontally = option;
}

void setMoreOption() {
    if (menu->selectedHorizontally == MORE) {
        increaseHorizontalOption();
    } else {
        selectHorizontalOption(MORE);
    }
}

void setLessOption() {
    if (menu->selectedHorizontally == LESS) {
        decreaseHorizontalOption();
    } else {
        selectHorizontalOption(LESS);
    }
}

void increaseHorizontalOption() {
    modifyHorizontalOption(1);
}

void decreaseHorizontalOption() {
    modifyHorizontalOption(-1);
}

void modifyHorizontalOption(int value) {
    int option = menu->selectedVertically;
    MenuOption &menuOption = menu->option[option];
    int currentValue = menuOption.value;

    if (currentValue + value <= menuOption.maxValue &&
        currentValue + value >= menuOption.minValue) {
        menuOption.value += value;
    }
}

void maintainEnterHorizontalOption() {
    if (menu->selectedHorizontally == LESS) {
        decreaseHorizontalOption();
    } else {
        increaseHorizontalOption();
    }
}

int getBombsLimitFromMenu(bool isSaveState) {
    int stateId = isSaveState ? SAVE_BOMBS_LIMIT : menu->selectedVertically;
    if (menu != nullptr) {
        int userSelectedBombAmount = (int) ((menu->option[stateId].value / 100.0) *
                                            (map->sizeX * map->sizeY - map->emptyFields));
        return max(userSelectedBombAmount, map->bombsLimit);
    }
    return -1;
}

string getSelectedOptionName() {
    return menu->option[menu->selectedVertically].title;
}
