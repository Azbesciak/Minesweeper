#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>

#include "Utils/Utils.h"
#include "Menu/Menu.h"



bool gameOver = false;

GameState maintainMainMenuState(ALLEGRO_EVENT *event, Menu *menu, ALLEGRO_FONT *menuFont) ;

GameState maintainMapEditorMenu(ALLEGRO_EVENT *event, Menu *menu, ALLEGRO_FONT *menuFont) ;

GameState maintainMapEditor(ALLEGRO_EVENT *event, Menu *menu, ALLEGRO_FONT *menuFont);

int main(int argc, char **argv) {
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *eventQueue = NULL;
    ALLEGRO_TIMER *timer = NULL;
    if (!al_init()) {
        al_show_native_message_box(NULL, NULL, NULL, "error occured while init allegro", NULL, 0);
        return -1;
    }
    display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display) {
        al_show_native_message_box(NULL, NULL, NULL, "error occured while creating display", NULL, 0);
        return -1;
    }

    bool shouldRedraw = false;

    al_init_primitives_addon();
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();

    al_hide_mouse_cursor(display);

    eventQueue = al_create_event_queue();
    ALLEGRO_FONT *menuFont = al_load_font("../pricedown bl.ttf", 36, 0);

    timer = al_create_timer(1.0 / FPS);

    al_register_event_source(eventQueue, al_get_keyboard_event_source());
    al_register_event_source(eventQueue, al_get_display_event_source(display));
    al_register_event_source(eventQueue, al_get_timer_event_source(timer));
    al_start_timer(timer);

    Menu *menu = new Menu();
    createMainMenu(menu);
    GameState gameState = STATE_MAIN_MENU;

    while (!gameOver) {
        ALLEGRO_EVENT event;
        GameState temp = gameState;
        al_wait_for_event(eventQueue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            shouldRedraw = true;
        }

        switch (gameState) {
            case STATE_MAIN_MENU:
                gameState = maintainMainMenuState(&event, menu, menuFont);
                break;
            case STATE_EDITOR_MENU:
                gameState = maintainMapEditorMenu(&event, menu, menuFont);
                break;
            case STATE_EDITOR:
                gameState = maintainMapEditor(&event, menu, menuFont);
                break;
            default: gameOver = true;
        }

        if (shouldRedraw && al_is_event_queue_empty(eventQueue)) {
            shouldRedraw = false;
            al_flip_display();
            al_clear_to_color(getMenuBackgroundColor());
        }
        if (temp != gameState) {
            switch (gameState) {
                case STATE_MAIN_MENU:
                    createMainMenu(menu); break;
                case STATE_EDITOR_MENU:
                     createMapEditorMenu(menu); break;
                default: gameOver = true;
                    clearMenu(menu);
            }
        }

    }

    al_destroy_timer(timer);
    al_destroy_font(menuFont);
    al_destroy_event_queue(eventQueue);
    al_destroy_display(display);

    return 0;
}

GameState maintainMainMenuState(ALLEGRO_EVENT *event, Menu *menu, ALLEGRO_FONT *menuFont) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_DOWN : setLowerOption(menu, MAIN_MENU_OPTIONS); break;
            case ALLEGRO_KEY_UP: setHigherOption(menu, MAIN_MENU_OPTIONS); break;
            case ALLEGRO_KEY_ESCAPE: gameOver = true; break;
            case ALLEGRO_KEY_ENTER: {
                int option = getSelectedOption(menu);
                if (option == MENU_QUIT) {
                    gameOver = true;
                } else if (option == MENU_START_GAME) {
                    return STATE_SELECT_MODE;
                } else if (option == MENU_EDITOR) {
                    return  STATE_EDITOR_MENU;
                }
                break;
            }
            default : break;
        }
    } else if (event->type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        gameOver = true;
    }
    displayMenu(menu, menuFont);
    return STATE_MAIN_MENU;
}

GameState maintainMapEditorMenu(ALLEGRO_EVENT *event, Menu *menu, ALLEGRO_FONT *menuFont) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_DOWN : setLowerOption(menu, EDITOR_MENU_OPTIONS); break;
            case ALLEGRO_KEY_UP: setHigherOption(menu, EDITOR_MENU_OPTIONS); break;
            case ALLEGRO_KEY_LEFT: setLessOption(menu); break;
            case ALLEGRO_KEY_RIGHT: setMoreOption(menu); break;
            case ALLEGRO_KEY_ESCAPE: return STATE_MAIN_MENU;
            case ALLEGRO_KEY_ENTER: {
                int option = getSelectedOption(menu);
                if (option == EDITOR_MENU_START) {
                    return STATE_EDITOR_MENU;
                } else if (option == EDITOR_MENU_CHOOSE_HEIGHT || option == EDITOR_MENU_CHOOSE_WIDTH) {
                    maintainEnterInEditorMenu(menu);
                } else {
                    return STATE_MAIN_MENU;
                }
                break;
            }
            default : break;
        }
    } else if (event->type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        gameOver = true;
    }
    displayMenu(menu, menuFont);
    return STATE_EDITOR_MENU;
}

GameState maintainMapEditor(ALLEGRO_EVENT *event, Menu *menu, ALLEGRO_FONT *menuFont) {
    return STATE_MAIN_MENU;
}