#include "../Utils/Utils.h"
#include "Menu.h"
#include <allegro5/allegro_primitives.h>


void createMainMenu(Menu *menu) {
    destroyMenu(menu);
    createMenu(menu, MAIN_MENU_OPTIONS);
    setOption(menu, MENU_START_GAME, "Play game");
    setOption(menu, MENU_EDITOR, "Editor");
    setOption(menu, MENU_QUIT, "Exit");
    selectVerticalOption(menu, MENU_START_GAME);
}

void createMapEditorMenu(Menu *menu) {
    destroyMenu(menu);
    createMenu(menu, EDITOR_MENU_OPTIONS);
    setOptionWithValues(menu, EDITOR_MENU_CHOOSE_HEIGHT, "Height", 10);
    setOptionWithValues(menu, EDITOR_MENU_CHOOSE_WIDTH, "Width", 10);
    setOption(menu, EDITOR_MENU_START, "Create");
    setOption(menu, EDITOR_MENU_QUIT, "Main menu");
    selectVerticalOption(menu, EDITOR_MENU_CHOOSE_HEIGHT);
    selectHorizontalOption(menu, MORE);
}

void createMenu(Menu *menu, int options) {
    menu = new Menu();
    menu->option = new string[options];
    menu->value = new int[options];
    menu ->options = options;
}

void destroyMenu(Menu *menu) {
    if (menu != nullptr) {
        delete[] menu->option;
        delete[] menu->value;
        menu->options = 0;
        delete menu;
    }
}

void setOption(Menu *menu, int optionId, string name) {
    menu->option[optionId] = name;
    menu->value[optionId] = -1;
}

void setOptionWithValues(Menu *menu, int optionId, string name, int value) {
    menu->option[optionId] = name;
    menu->value[optionId] = value;
}

void displayMenu(Menu *menu, ALLEGRO_FONT *font) {
    ALLEGRO_COLOR HIGHLIGHT_COLOR = getMenuHighlightColor();
    ALLEGRO_COLOR NORMAL_COLOR = getMenuNormalColor();
    for (int option = 0; option < menu->options; option++) {
        int lineX = SCREEN_WIDTH / 2;
        int lineY = SCREEN_HEIGHT / 2 + option * MENU_ITEM_HEIGHT - (int)(menu->options / 2.0 * MENU_ITEM_HEIGHT);
        ALLEGRO_COLOR color = menu->selectedVertically == option ? HIGHLIGHT_COLOR : NORMAL_COLOR;

        if (menu->value[option] >= 0 ) {
            al_draw_text(font, color, lineX, lineY, ALLEGRO_ALIGN_RIGHT, menu->option[option].c_str());
            ALLEGRO_COLOR triangleColor =
                    menu->selectedVertically == option && menu->selectedHorizontally == LESS ? getArrowHighlightColor()
                                                                                             : color;
            al_draw_filled_triangle(SCREEN_WIDTH / 2 + 20, lineY + 25,
                                    SCREEN_WIDTH / 2 + 35, lineY + 15,
                                    SCREEN_WIDTH / 2 + 35, lineY + 35, triangleColor);

            al_draw_textf(font, color, SCREEN_WIDTH / 2 + 60, lineY, ALLEGRO_ALIGN_CENTER, "%i", menu->value[option]);

            triangleColor =
                    menu->selectedVertically == option && menu->selectedHorizontally == MORE ? getArrowHighlightColor()
                                                                                             : color;
            al_draw_filled_triangle(SCREEN_WIDTH / 2 + 85, lineY + 15,
                                    SCREEN_WIDTH / 2 + 100, lineY + 25,
                                    SCREEN_WIDTH / 2 + 85, lineY + 35, triangleColor);
        } else {
            al_draw_text(font, color, lineX, lineY, ALLEGRO_ALIGN_CENTER, menu->option[option].c_str());
        }
    }
}

void setLowerOption(Menu *menu, int maxLength) {
    int nextOption = (menu->selectedVertically + 1) % maxLength;
    selectVerticalOption(menu, nextOption);
}

void setHigherOption(Menu *menu, int maxLength) {
    int nextOption = menu->selectedVertically - 1 < 0 ? maxLength - 1 : menu->selectedVertically - 1;
    selectVerticalOption(menu, nextOption);
}

void selectVerticalOption(Menu *menu, int optionId) {
    menu->selectedVertically = optionId;
}

int getSelectedOption(Menu *menu) {
    return menu->selectedVertically;
}

void selectHorizontalOption(Menu *menu, int option) {
    menu->selectedHorizontally = option;
}

void setMoreOption(Menu *menu) {
    if (menu->selectedHorizontally == MORE) {
        increaseHorizontalOption(menu);
    } else {
        selectHorizontalOption(menu, MORE);
    }
}

void setLessOption(Menu *menu) {
    if (menu->selectedHorizontally == LESS) {
        decreaseHorizontalOption(menu);
    } else {
        selectHorizontalOption(menu, LESS);
    }
}

void increaseHorizontalOption(Menu *menu) {
    modifyHorizontalOption(menu, 1);
}

void decreaseHorizontalOption(Menu *menu) {
    modifyHorizontalOption(menu, -1);
}

void modifyHorizontalOption(Menu *menu, int value) {
    int option = menu->selectedVertically;
    int currentValue = menu->value[option];

    if (currentValue + value <= MAX_FIELDS && currentValue + value >= MIN_FIELDS) {
        menu->value[option] = currentValue + value;
    }
}

void maintainEnterInEditorMenu(Menu *menu) {
    if (menu->selectedHorizontally == LESS) {
        decreaseHorizontalOption(menu);
    } else {
        increaseHorizontalOption(menu);
    }
}
