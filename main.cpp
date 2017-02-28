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
State maintainMainMenuState(ALLEGRO_EVENT *event);

State maintainMapEditorMenu(ALLEGRO_EVENT *event);

State maintainMapEditor(ALLEGRO_EVENT *event);

State maintainEditorPauseMenu(ALLEGRO_EVENT *event);

State maintainSaveMapMenu(ALLEGRO_EVENT *event, ALLEGRO_DISPLAY *display);

State maintainSelectMapState(ALLEGRO_EVENT *event, ALLEGRO_DISPLAY *display);

State maintainSelectDifficultyState(ALLEGRO_EVENT *event);

State maintainGameState(ALLEGRO_EVENT *event);

State maintainGamePauseMenuState(ALLEGRO_EVENT *event);

State maintainGameFinishedState(ALLEGRO_EVENT *pEVENT);

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
                    showMouse(display);
                    if (temp != STATE_EDITOR_PAUSE) {
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
                        resetScore();
                        resetTimer();
                    }
                    showMouse(display);
                    break;
                case STATE_GAME_PAUSE:
                    hideMouse(display);
                    createGamePauseMenu();
                    break;
                case STATE_GAME_FINISHED:
                    hideMouse(display);
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
}

State maintainMainMenuState(ALLEGRO_EVENT *event) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_DOWN : setLowerOption(MAIN_MENU_OPTIONS); break;
            case ALLEGRO_KEY_UP: setHigherOption(MAIN_MENU_OPTIONS); break;
            case ALLEGRO_KEY_ESCAPE: gameOver = true; break;
            case ALLEGRO_KEY_ENTER: {
                int option = getSelectedOption();
                switch (option) {
                    case MENU_QUIT: gameOver = true; break;
                    case MENU_START_GAME: return STATE_SELECT_MAP;
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

State maintainMapEditorMenu(ALLEGRO_EVENT *event) {
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
                    case EDITOR_MENU_CHOOSE_WIDTH:
                        maintainEnterHorizontalOption(); break;
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

State maintainMapEditor(ALLEGRO_EVENT *event) {
        if (event->keyboard.keycode == ALLEGRO_KEY_ESCAPE && event->type == ALLEGRO_EVENT_KEY_DOWN) {
            return STATE_EDITOR_PAUSE;
        } else if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            maintainEditor(event);
        }
        displayMap(false);
    return STATE_EDITOR;
}

State maintainEditorPauseMenu(ALLEGRO_EVENT *event) {
   if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_DOWN : setLowerOption(EDITOR_MENU_OPTIONS); break;
            case ALLEGRO_KEY_UP: setHigherOption(EDITOR_MENU_OPTIONS); break;
            case ALLEGRO_KEY_ESCAPE: return STATE_MAIN_MENU;
            case ALLEGRO_KEY_ENTER: {
                int option = getSelectedOption();
                switch (option) {
                    case EDITOR_PAUSE_CONTINUE: return STATE_EDITOR;
                    case EDITOR_PAUSE_SAVE: return STATE_SAVE_MAP;
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

State maintainSaveMapMenu(ALLEGRO_EVENT *event, ALLEGRO_DISPLAY *display) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_DOWN : setLowerOption(SAVE_MAP_OPTIONS); break;
            case ALLEGRO_KEY_UP: setHigherOption(SAVE_MAP_OPTIONS); break;
            case ALLEGRO_KEY_LEFT: setLessOption(); break;
            case ALLEGRO_KEY_RIGHT: setMoreOption(); break;
            case ALLEGRO_KEY_ESCAPE: return STATE_EDITOR_PAUSE;
            case ALLEGRO_KEY_ENTER: {
                int option = getSelectedOption();
                switch (option) {
                    case SAVE_BOMBS_LIMIT: maintainEnterHorizontalOption();break;

                    case SAVE_SAVE: {
                        bool wasPersisted = persistMap(display);
                        if (wasPersisted) cout << "map persisted" <<endl;
                        else cout << "not persisted" << endl;
                        return STATE_EDITOR_MENU;
                    }
                    case SAVE_QUIT: return STATE_EDITOR_PAUSE;
                    default : return STATE_MAIN_MENU;
                }
                break;
            }
            default : break;
        }
    }
    displayMenu();
    return STATE_SAVE_MAP;
}

State maintainSelectMapState(ALLEGRO_EVENT *event, ALLEGRO_DISPLAY *display) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_DOWN : setLowerOption(SELECT_MAP_OPTIONS); break;
            case ALLEGRO_KEY_UP: setHigherOption(SELECT_MAP_OPTIONS); break;
            case ALLEGRO_KEY_ESCAPE: return STATE_MAIN_MENU;
            case ALLEGRO_KEY_ENTER: {
                int option = getSelectedOption();
                bool isCustom = false;
                bool isSelected;
                switch (option) {
                    case SELECT_CUSTOM:
                        isCustom = true;
                    case SELECT_10X10: case SELECT_25X25: case SELECT_50X50: {
                        isSelected = selectMap(isCustom, display);
                        if (isSelected) {
                            cout << "map selected" <<endl;
                            return STATE_SELECT_DIFFICULTY;
                        } else {
                            cout<<"map NOT selected" <<endl;
                            return STATE_SELECT_MAP;
                        }
                    }
                    case SELECT_BACK:
                    default : return STATE_MAIN_MENU;
                }
            }
            default : break;
        }
    }
    displayMenu();
    return STATE_SELECT_MAP;
}

State maintainSelectDifficultyState(ALLEGRO_EVENT *event) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_DOWN : setLowerOption(LEVEL_DIFFICULTY_OPTIONS); break;
            case ALLEGRO_KEY_UP: setHigherOption(LEVEL_DIFFICULTY_OPTIONS); break;
            case ALLEGRO_KEY_ESCAPE: return STATE_MAIN_MENU;
            case ALLEGRO_KEY_ENTER: {
                int option = getSelectedOption();
                switch (option) {
                    case DIF_EASY: case DIF_NORMAL: case DIF_HARD: case DIF_CUSTOM:
                        setBombsLimit(false);
                        return STATE_GAME;
                    case DIF_QUIT: return STATE_SELECT_MAP;
                    default : return STATE_MAIN_MENU;
                }
            }
            default : break;
        }
    }
    displayMenu();
    return STATE_SELECT_DIFFICULTY;
}

State maintainGameState(ALLEGRO_EVENT *event) {
    bool gameInProgress = true;
    if (event->keyboard.keycode == ALLEGRO_KEY_ESCAPE && event->type == ALLEGRO_EVENT_KEY_DOWN) {
        return STATE_GAME_PAUSE;
    } else if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        gameInProgress = maintainGame(event);
    } else if (event->type == ALLEGRO_EVENT_TIMER) {
        incrementTimer();
    }
    displayMap(true);
    if (gameInProgress) {
        return STATE_GAME;
    } else {
        return STATE_GAME_FINISHED;
    }
}

State maintainGamePauseMenuState(ALLEGRO_EVENT *event) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_DOWN : setLowerOption(GAME_PAUSE_OPTIONS); break;
            case ALLEGRO_KEY_UP: setHigherOption(GAME_PAUSE_OPTIONS); break;
            case ALLEGRO_KEY_ESCAPE: return STATE_MAIN_MENU;
            case ALLEGRO_KEY_ENTER: {
                int option = getSelectedOption();
                switch (option) {
                    case GAME_PAUSE_CONTINUE: return STATE_GAME;
                    case GAME_PAUSE_QUIT:
                    default : return STATE_MAIN_MENU;
                }
            }
            default : break;
        }
    }
    displayMenu();
    return STATE_GAME_PAUSE;
}

State maintainGameFinishedState(ALLEGRO_EVENT *event) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
//            case ALLEGRO_KEY_LEFT : setLowerOption(GAME_PAUSE_OPTIONS); break;
//            case ALLEGRO_KEY_RIGHT: setHigherOption(GAME_PAUSE_OPTIONS); break;
            case ALLEGRO_KEY_ESCAPE:
            case ALLEGRO_KEY_ENTER: return STATE_MAIN_MENU;

//            {
//                int option = getSelectedOption();
//                switch (option) {
//                    case GAME_PAUSE_CONTINUE: return STATE_GAME;
//                    case GAME_PAUSE_QUIT:
//                    default : return STATE_MAIN_MENU;
//                }
//            }
            default : break;
        }
    }
    displayMap(true);
    displayGameResult();
//    displayMenu();
    return STATE_GAME_FINISHED;
}

void hideMouse(ALLEGRO_DISPLAY *display) {
    al_hide_mouse_cursor(display);
    mouseVisible = false;
}
void showMouse(ALLEGRO_DISPLAY *display) {
    al_show_mouse_cursor(display);
    mouseVisible = true;
}