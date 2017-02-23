#include "../Utils/Utils.h"
#include "Menu.h"
#include <allegro5/allegro_primitives.h>


Menu createMainMenu() {
    Menu menu;
    menu.option[MENU_START_GAME] = "Play game";
    menu.option[MENU_EDITOR] = "Editor";
    menu.option[MENU_QUIT] = "Exit";
    menu.selectedVertically = MENU_START_GAME;
    return menu;
}

Menu createMapEditorMenu() {
    Menu menu;
    menu.option[EDITOR_MENU_CHOOSE_HEIGHT] = "Height";
    menu.option[EDITOR_MENU_CHOOSE_WIDTH] = "Width";
    menu.option[EDITOR_MENU_START] = "Create";
    menu.option[EDITOR_MENU_QUIT] = "Main Menu";
    menu.selectedVertically = EDITOR_MENU_CHOOSE_HEIGHT;
    menu.selectedHorizontally = MORE;
    menu.values[EDITOR_MENU_CHOOSE_HEIGHT] = 10;
    menu.values[EDITOR_MENU_CHOOSE_WIDTH] = 10;
    return menu;
}

void displayMainMenu(Menu *menu, ALLEGRO_FONT *font) {
    ALLEGRO_COLOR HIGHLIGHT_COLOR = getMenuHighlightColor();
    ALLEGRO_COLOR NORMAL_COLOR = getMenuNormalColor();
    for (int option = 0; option < MAIN_MENU_OPTIONS; option++) {
        ALLEGRO_COLOR color = menu->selectedVertically == option ? HIGHLIGHT_COLOR : NORMAL_COLOR;
        al_draw_text(font, color, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + option * MENU_ITEM_HEIGHT - MENU_ITEM_HEIGHT * 2,
                     ALLEGRO_ALIGN_CENTER, menu->option[option].c_str());
    }

}

void displayEditorMenu(Menu *menu, ALLEGRO_FONT *font) {
    ALLEGRO_COLOR HIGHLIGHT_COLOR = getMenuHighlightColor();
    ALLEGRO_COLOR NORMAL_COLOR = getMenuNormalColor();
    for (int option = 0; option < EDITOR_MENU_OPTIONS; option++) {
        int lineX = SCREEN_WIDTH / 2;
        int lineY = SCREEN_HEIGHT / 2 + option * MENU_ITEM_HEIGHT - MENU_ITEM_HEIGHT * 2;
        ALLEGRO_COLOR color = menu->selectedVertically == option ? HIGHLIGHT_COLOR : NORMAL_COLOR;

        if (option == EDITOR_MENU_CHOOSE_WIDTH || option == EDITOR_MENU_CHOOSE_HEIGHT) {
            al_draw_text(font, color, lineX, lineY, ALLEGRO_ALIGN_RIGHT, menu->option[option].c_str());
            ALLEGRO_COLOR triangleColor = menu->selectedVertically == option && menu->selectedHorizontally == LESS ? getArrowHighlightColor() : color;
            al_draw_filled_triangle(SCREEN_WIDTH / 2 + 20, lineY + 25,
                                    SCREEN_WIDTH / 2 + 35, lineY + 15,
                                    SCREEN_WIDTH / 2 + 35, lineY + 35, triangleColor);

            al_draw_textf(font, color, SCREEN_WIDTH / 2 + 60, lineY, ALLEGRO_ALIGN_CENTER, "%i", menu->values[option]);

            triangleColor = menu->selectedVertically == option && menu->selectedHorizontally == MORE ? getArrowHighlightColor() : color;
            al_draw_filled_triangle( SCREEN_WIDTH / 2 + 85, lineY + 15,
                                    SCREEN_WIDTH / 2 + 100, lineY + 25,
                                     SCREEN_WIDTH / 2 + 85, lineY + 35, triangleColor);
        } else {
            al_draw_text(font, color, lineX, lineY, ALLEGRO_ALIGN_CENTER, menu->option[option].c_str());
        }
    }
}

void setLowerOption(Menu *menu, int maxLength) {
    int nextOption = (menu->selectedVertically + 1) % maxLength;
    setOption(menu, nextOption);
}

void setHigherOption(Menu *menu, int maxLength) {
    int nextOption = menu->selectedVertically - 1 < 0 ? maxLength - 1 : menu->selectedVertically - 1 ;
    setOption(menu, nextOption);
}

void setOption(Menu *menu, int optionId) {
    menu->selectedVertically = optionId;
}

int getSelectedOption(Menu *menu) {
    return menu->selectedVertically;
}

void setMoreOption(Menu *menu) {
    if (menu->selectedHorizontally == MORE) {
        increaseHorizontalOption(menu);
    } else {
        menu->selectedHorizontally = MORE;
    }
}

void setLessOption(Menu *menu) {
    if (menu->selectedHorizontally == LESS) {
        decreaseHorizontalOption(menu);
    } else {
        menu->selectedHorizontally = LESS;
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
    int currentValue = menu->values[option];

    if (currentValue + value <= MAX_FIELDS && currentValue + value >= MIN_FIELDS) {
        menu->values[option] = currentValue + value;
    }
}

void maintainEnterInEditorMenu(Menu *menu) {
    if (menu->selectedHorizontally == LESS) {
        decreaseHorizontalOption(menu);
    } else {
        increaseHorizontalOption(menu);
    }
}
