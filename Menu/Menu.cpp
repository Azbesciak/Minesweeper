#include "../Utils/Utils.h"
#include "Menu.h"
#include "../Map/Map.h"
#include <allegro5/allegro_primitives.h>

Menu *menu = nullptr;
ALLEGRO_FONT *menuFont = nullptr;
void initializeMenuFont() {
    menuFont = al_load_font("../pricedown bl.ttf", 36, 0);
}
void destroyMenuFont() {
    al_destroy_font(menuFont);
}

void createMainMenu() {
    destroyMenu();
    createMenu(MAIN_MENU_OPTIONS);
    setOption(MENU_START_GAME, "Play game");
    setOption(MENU_EDITOR, "Editor");
    setOption( MENU_QUIT, "Exit");
    selectVerticalOption(MENU_START_GAME);
}

void createMapEditorMenu() {
    destroyMenu();
    createMenu(EDITOR_MENU_OPTIONS);
    setOptionWithValues(EDITOR_MENU_CHOOSE_HEIGHT, "Height", 10);
    setOptionWithValues(EDITOR_MENU_CHOOSE_WIDTH, "Width", 10);
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

void createMenu(int options) {
    menu = new Menu();
    menu->option = new string[options];
    menu->value = new int[options];
    menu->options = options;
}

void destroyMenu() {
    if (menu != nullptr) {
        if (menu->option != nullptr) {
            delete[] menu->option;
        }
        if (menu->value != nullptr) {
            delete[] menu->value;
        }
        delete menu;
    }
}

void setOption(int optionId, string name) {
    menu->option[optionId] = name;
    menu->value[optionId] = -1;
}

void setOptionWithValues(int optionId, string name, int value) {
    menu->option[optionId] = name;
    menu->value[optionId] = value;
}

void displayMenu() {
    ALLEGRO_COLOR HIGHLIGHT_COLOR = getMenuHighlightColor();
    ALLEGRO_COLOR NORMAL_COLOR = getMenuNormalColor();
    for (int option = 0; option < menu->options; option++) {
        int lineX = SCREEN_WIDTH / 2;
        int lineY = SCREEN_HEIGHT / 2 + option * MENU_ITEM_HEIGHT - (int)(menu->options / 2.0 * MENU_ITEM_HEIGHT);
        ALLEGRO_COLOR color = menu->selectedVertically == option ? HIGHLIGHT_COLOR : NORMAL_COLOR;

        if (menu->value[option] >= 0 ) {
            al_draw_text(menuFont, color, lineX, lineY, ALLEGRO_ALIGN_RIGHT, menu->option[option].c_str());
            ALLEGRO_COLOR triangleColor =
                    menu->selectedVertically == option && menu->selectedHorizontally == LESS ? getArrowHighlightColor()
                                                                                             : color;
            al_draw_filled_triangle(SCREEN_WIDTH / 2 + 20, lineY + 25,
                                    SCREEN_WIDTH / 2 + 35, lineY + 15,
                                    SCREEN_WIDTH / 2 + 35, lineY + 35, triangleColor);

            al_draw_textf(menuFont, color, SCREEN_WIDTH / 2 + 60, lineY, ALLEGRO_ALIGN_CENTER, "%i", menu->value[option]);

            triangleColor =
                    menu->selectedVertically == option && menu->selectedHorizontally == MORE ? getArrowHighlightColor()
                                                                                             : color;
            al_draw_filled_triangle(SCREEN_WIDTH / 2 + 85, lineY + 15,
                                    SCREEN_WIDTH / 2 + 100, lineY + 25,
                                    SCREEN_WIDTH / 2 + 85, lineY + 35, triangleColor);
        } else {
            al_draw_text(menuFont, color, lineX, lineY, ALLEGRO_ALIGN_CENTER, menu->option[option].c_str());
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
    int currentValue = menu->value[option];

    if (currentValue + value <= MAX_FIELDS && currentValue + value >= MIN_FIELDS) {
        menu->value[option] = currentValue + value;
    }
}

void maintainEnterInEditorMenu() {
    if (menu->selectedHorizontally == LESS) {
        decreaseHorizontalOption();
    } else {
        increaseHorizontalOption();
    }
}
