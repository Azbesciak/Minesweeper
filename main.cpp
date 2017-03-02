#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>

#include "Utils/Utils.h"
#include "Functional/Menu/Menu.h"
#include "Functional/Map/Map.h"
#include "Functional/Ranking/Ranking.h"
#include "Game/Logic/GameLogic.h"


void initializeAll();
void destroyAll();

void hideMouse(ALLEGRO_DISPLAY *display);
void showMouse(ALLEGRO_DISPLAY *display);


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
    State gameState = STATE_MAIN_MENU;

    while (!gameOver) {
        ALLEGRO_EVENT event;
        State temp = gameState;
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
                gameState = maintainEditorPauseMenu(&event);
                break;
            case STATE_SAVE_MAP:
                gameState = maintainSaveMapMenu(&event, display);
                break;
            case STATE_SELECT_MAP:
                gameState = maintainSelectMapState(&event, display);
                break;
            case STATE_SELECT_DIFFICULTY:
                gameState = maintainSelectDifficultyState(&event);
                break;
            case STATE_GAME:
                gameState = maintainGameState(&event);
                break;
            case STATE_GAME_PAUSE:
                gameState = maintainGamePauseMenuState(&event);
                break;
            case STATE_GAME_FINISHED:
                gameState = maintainGameFinishedState(&event);
                break;
            case STATE_SHOW_RANKING:
                gameState = maintainShowStatsState(&event);
                break;
            case STATE_SAVE_STATS:
                gameState = maintainSaveResultState(&event);
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
                    if (temp == STATE_SHOW_RANKING) {
                        destroyStats();
                    } else if (temp == STATE_SAVE_STATS) {
                        destroyLastPlayerStats();
                    }
                    createMainMenu(); break;
                case STATE_EDITOR_MENU:
                    if (temp == STATE_EDITOR) {
                        hideMouse(display);
                    }
                     createMapEditorMenu(); break;
                case STATE_EDITOR:
                    showMouse(display);
                    if (temp != STATE_EDITOR_PAUSE) {
                        initializeGameLogicProps();
                        initializeEmptyMap(menu->option[EDITOR_MENU_CHOOSE_WIDTH].value,
                                           menu->option[EDITOR_MENU_CHOOSE_HEIGHT].value);
                    }
                    break;
                case STATE_EDITOR_PAUSE:
                    hideMouse(display);
                    createMapEditorPauseMenu(); break;
                case STATE_SAVE_MAP:
                    createSaveMapMenu();break;
                case STATE_SELECT_MAP:
                    createSelectMapMenu(); break;
                case STATE_SELECT_DIFFICULTY:
                    createLevelDifficultyMenu(); break;
                case STATE_GAME:
                    if (temp == STATE_SELECT_DIFFICULTY) {
                        initializeGameLogicProps();
                    }
                    showMouse(display);
                    break;
                case STATE_GAME_PAUSE:
                    hideMouse(display);
                    createGamePauseMenu();
                    break;
                case STATE_GAME_FINISHED:
                    hideMouse(display);
                    createGameFinishedMenu();
                    break;
                case STATE_SAVE_STATS:
                    createLastPlayerStats();
                    break;
                case STATE_SHOW_RANKING:
                    initStats();
                    break;

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
    destroyGameLogicProps();
}

void hideMouse(ALLEGRO_DISPLAY *display) {
    al_hide_mouse_cursor(display);
}
void showMouse(ALLEGRO_DISPLAY *display) {
    al_show_mouse_cursor(display);
}