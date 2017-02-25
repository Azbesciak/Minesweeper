#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>

#include "Utils/Utils.h"
#include "Menu/Menu.h"
#include "Map/Map.h"


bool gameOver = false;
bool mouseVisible = false;
void initializeAll();
void destroyAll();

void hideMouse(ALLEGRO_DISPLAY *display);
void showMouse(ALLEGRO_DISPLAY *display);
GameState maintainMainMenuState(ALLEGRO_EVENT *event);

GameState maintainMapEditorMenu(ALLEGRO_EVENT *event);

GameState maintainMapEditor(ALLEGRO_EVENT *event);

GameState maintainEditorPauseMenu(ALLEGRO_EVENT *event, ALLEGRO_DISPLAY *display);

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
    al_install_mouse();

    eventQueue = al_create_event_queue();


    timer = al_create_timer(1.0 / FPS);

    al_register_event_source(eventQueue, al_get_keyboard_event_source());
    al_register_event_source(eventQueue, al_get_display_event_source(display));
    al_register_event_source(eventQueue, al_get_timer_event_source(timer));
    al_register_event_source(eventQueue, al_get_mouse_event_source());
    al_start_timer(timer);
    initializeAll();
    GameState gameState = STATE_MAIN_MENU;

    while (!gameOver) {
        ALLEGRO_EVENT event;
        GameState temp = gameState;
        al_wait_for_event(eventQueue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            shouldRedraw = true;
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            gameOver = true;
            break;
        }

        switch (gameState) {
            case STATE_MAIN_MENU:
                gameState = maintainMainMenuState(&event);
                break;
            case STATE_EDITOR_MENU:
                gameState = maintainMapEditorMenu(&event);
                break;
            case STATE_EDITOR:
                gameState = maintainMapEditor(&event);
                break;
            case STATE_EDITOR_PAUSE:
                gameState = maintainEditorPauseMenu(&event, display);
                break;
            default: gameOver = true;
        }

        if (shouldRedraw && al_is_event_queue_empty(eventQueue)) {
            shouldRedraw = false;
            al_flip_display();
            al_clear_to_color(getBackgroundColor());
        }
        // if the state has changed
        if (temp != gameState) {
            switch (gameState) {
                case STATE_MAIN_MENU:
                    createMainMenu(); break;
                case STATE_EDITOR_MENU:
                    if (temp == STATE_EDITOR) {
                        hideMouse(display);
                    }
                     createMapEditorMenu(); break;
                case STATE_EDITOR:
                    destroyMap();
                    showMouse(display);
                    if (temp != STATE_EDITOR_PAUSE) {
                        initializeEmptyMap(menu->value[EDITOR_MENU_CHOOSE_WIDTH], menu->value[EDITOR_MENU_CHOOSE_HEIGHT]);
                    }
                    break;
                case STATE_EDITOR_PAUSE:
                    hideMouse(display);
                    createMapEditorPauseMenu(); break;
                default: gameOver = true;
            }
        }

    }
    destroyAll();
    al_destroy_timer(timer);
    al_destroy_event_queue(eventQueue);
    al_destroy_display(display);

    return 0;
}

void initializeAll() {
    initializeMenuFont();
    initializeColors();
    createMainMenu();
}
void destroyAll() {
    destroyColors();
    destroyMap();
    destroyMenu();
    destroyMenuFont();
}

GameState maintainMainMenuState(ALLEGRO_EVENT *event) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_DOWN : setLowerOption(MAIN_MENU_OPTIONS); break;
            case ALLEGRO_KEY_UP: setHigherOption(MAIN_MENU_OPTIONS); break;
            case ALLEGRO_KEY_ESCAPE: gameOver = true; break;
            case ALLEGRO_KEY_ENTER: {
                int option = getSelectedOption();
                switch (option) {
                    case MENU_QUIT: gameOver = true; break;
                    case MENU_START_GAME: return STATE_SELECT_MODE;
                    case MENU_EDITOR: return STATE_EDITOR_MENU;
                    default : break;
                }
            }
            default : break;
        }
    } else if (event->type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        gameOver = true;
    }
    displayMenu();
    return STATE_MAIN_MENU;
}

GameState maintainMapEditorMenu(ALLEGRO_EVENT *event) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_DOWN : setLowerOption(EDITOR_MENU_OPTIONS); break;
            case ALLEGRO_KEY_UP: setHigherOption( EDITOR_MENU_OPTIONS); break;
            case ALLEGRO_KEY_LEFT: setLessOption(); break;
            case ALLEGRO_KEY_RIGHT: setMoreOption(); break;
            case ALLEGRO_KEY_ESCAPE: return STATE_MAIN_MENU;
            case ALLEGRO_KEY_ENTER: {
                int option = getSelectedOption();
                switch (option) {
                    case EDITOR_MENU_START: return STATE_EDITOR;
                    case EDITOR_MENU_CHOOSE_HEIGHT:
                    case EDITOR_MENU_CHOOSE_WIDTH: maintainEnterInEditorMenu(); break;
                    default : return STATE_MAIN_MENU;
                }
                break;
            }
            default : break;
        }
    }
    displayMenu();
    return STATE_EDITOR_MENU;
}

GameState maintainMapEditor(ALLEGRO_EVENT *event) {
        if (event->keyboard.keycode == ALLEGRO_KEY_ESCAPE && event->type == ALLEGRO_EVENT_KEY_DOWN) {
            return STATE_EDITOR_PAUSE;
        } else if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            changeClickedFieldState(event);
        }
        displayMap();
    return STATE_EDITOR;
}

GameState maintainEditorPauseMenu(ALLEGRO_EVENT *event, ALLEGRO_DISPLAY *display) {
   if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_DOWN : setLowerOption(EDITOR_MENU_OPTIONS); break;
            case ALLEGRO_KEY_UP: setHigherOption(EDITOR_MENU_OPTIONS); break;
            case ALLEGRO_KEY_ESCAPE: return STATE_MAIN_MENU;
            case ALLEGRO_KEY_ENTER: {
                int option = getSelectedOption();
                switch (option) {
                    case EDITOR_PAUSE_CONTINUE: return STATE_EDITOR;
                    case EDITOR_PAUSE_SAVE: {
                        bool wasPersisted = persistMap(display);
                        return STATE_EDITOR_MENU;
                    }
                    case EDITOR_PAUSE_RESET: return STATE_EDITOR_MENU;
                    default : return STATE_MAIN_MENU;
                }
            }
            default : break;
        }
    }
    displayMenu();
    return STATE_EDITOR_PAUSE;
}

void hideMouse(ALLEGRO_DISPLAY *display) {
    al_hide_mouse_cursor(display);
    mouseVisible = false;
}
void showMouse(ALLEGRO_DISPLAY *display) {
    al_show_mouse_cursor(display);
    mouseVisible = true;
}