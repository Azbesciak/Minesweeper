
#include "GameStates.h"
#include "../../Functional/Ranking/Ranking.h"
#include "../../Functional/Map/Map.h"
#include "../../Functional/Menu/Menu.h"
#include "../Logic/GameLogic.h"

bool gameOver;

State maintainMainMenuState(ALLEGRO_EVENT *event) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_DOWN :
                setLowerOption(MAIN_MENU_OPTIONS);
                break;
            case ALLEGRO_KEY_UP:
                setHigherOption(MAIN_MENU_OPTIONS);
                break;
            case ALLEGRO_KEY_ESCAPE:
                gameOver = true;
                break;
            case ALLEGRO_KEY_ENTER: {
                int option = getSelectedOption();
                switch (option) {
                    case MENU_QUIT:
                        gameOver = true;
                        break;
                    case MENU_START_GAME:
                        return STATE_SELECT_MAP;
                    case MENU_EDITOR:
                        return STATE_EDITOR_MENU;
                    case MENU_RANKING:
                        return STATE_SHOW_RANKING;
                    default :
                        break;
                }
            }
            default :
                break;
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
            case ALLEGRO_KEY_DOWN :
                setLowerOption(EDITOR_MENU_OPTIONS);
                break;
            case ALLEGRO_KEY_UP:
                setHigherOption(EDITOR_MENU_OPTIONS);
                break;
            case ALLEGRO_KEY_LEFT:
                setLessOption();
                break;
            case ALLEGRO_KEY_RIGHT:
                setMoreOption();
                break;
            case ALLEGRO_KEY_ESCAPE:
                return STATE_MAIN_MENU;
            case ALLEGRO_KEY_ENTER: {
                int option = getSelectedOption();
                switch (option) {
                    case EDITOR_MENU_START:
                        return STATE_EDITOR;
                    case EDITOR_MENU_CHOOSE_HEIGHT:
                    case EDITOR_MENU_CHOOSE_WIDTH:
                        maintainEnterHorizontalOption();
                        break;
                    default :
                        return STATE_MAIN_MENU;
                }
                break;
            }
            default :
                break;
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
            case ALLEGRO_KEY_DOWN :
                setLowerOption(EDITOR_MENU_OPTIONS);
                break;
            case ALLEGRO_KEY_UP:
                setHigherOption(EDITOR_MENU_OPTIONS);
                break;
            case ALLEGRO_KEY_ESCAPE:
                return STATE_MAIN_MENU;
            case ALLEGRO_KEY_ENTER: {
                int option = getSelectedOption();
                switch (option) {
                    case EDITOR_PAUSE_CONTINUE:
                        return STATE_EDITOR;
                    case EDITOR_PAUSE_SAVE:
                        return STATE_SAVE_MAP;
                    case EDITOR_PAUSE_RESET:
                        return STATE_EDITOR_MENU;
                    default :
                        return STATE_MAIN_MENU;
                }
            }
            default :
                break;
        }
    }
    displayMenu();
    return STATE_EDITOR_PAUSE;
}

State maintainSaveMapMenu(ALLEGRO_EVENT *event, ALLEGRO_DISPLAY *display) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_DOWN :
                setLowerOption(SAVE_MAP_OPTIONS);
                break;
            case ALLEGRO_KEY_UP:
                setHigherOption(SAVE_MAP_OPTIONS);
                break;
            case ALLEGRO_KEY_LEFT:
                setLessOption();
                break;
            case ALLEGRO_KEY_RIGHT:
                setMoreOption();
                break;
            case ALLEGRO_KEY_ESCAPE:
                return STATE_EDITOR_PAUSE;
            case ALLEGRO_KEY_ENTER: {
                int option = getSelectedOption();
                switch (option) {
                    case SAVE_BOMBS_LIMIT:
                        maintainEnterHorizontalOption();
                        break;

                    case SAVE_SAVE: {
                        bool wasPersisted = persistMap(display);
                        if (wasPersisted) cout << "map persisted" << endl;
                        else cout << "not persisted" << endl;
                        return STATE_EDITOR_MENU;
                    }
                    case SAVE_QUIT:
                        return STATE_EDITOR_PAUSE;
                    default :
                        return STATE_MAIN_MENU;
                }
                break;
            }
            default :
                break;
        }
    }
    displayMenu();
    return STATE_SAVE_MAP;
}

State maintainSelectMapState(ALLEGRO_EVENT *event, ALLEGRO_DISPLAY *display) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_DOWN :
                setLowerOption(SELECT_MAP_OPTIONS);
                break;
            case ALLEGRO_KEY_UP:
                setHigherOption(SELECT_MAP_OPTIONS);
                break;
            case ALLEGRO_KEY_ESCAPE:
                return STATE_MAIN_MENU;
            case ALLEGRO_KEY_ENTER: {
                int option = getSelectedOption();
                bool isCustom = false;
                bool isSelected;
                switch (option) {
                    case SELECT_CUSTOM:
                        isCustom = true;
                    case SELECT_10X10:
                    case SELECT_25X25:
                    case SELECT_50X50: {
                        isSelected = selectMap(isCustom, display);
                        if (isSelected) {
                            cout << "map selected" << endl;
                            return STATE_SELECT_DIFFICULTY;
                        } else {
                            cout << "map NOT selected" << endl;
                            return STATE_SELECT_MAP;
                        }
                    }
                    case SELECT_BACK:
                    default :
                        return STATE_MAIN_MENU;
                }
            }
            default :
                break;
        }
    }
    displayMenu();
    return STATE_SELECT_MAP;
}

State maintainSelectDifficultyState(ALLEGRO_EVENT *event) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_DOWN :
                setLowerOption(LEVEL_DIFFICULTY_OPTIONS);
                break;
            case ALLEGRO_KEY_UP:
                setHigherOption(LEVEL_DIFFICULTY_OPTIONS);
                break;
            case ALLEGRO_KEY_ESCAPE:
                return STATE_MAIN_MENU;
            case ALLEGRO_KEY_ENTER: {
                int option = getSelectedOption();
                switch (option) {
                    case DIF_EASY:
                    case DIF_NORMAL:
                    case DIF_HARD:
                    case DIF_CUSTOM:
                        setBombsLimit(false);
                        return STATE_GAME;
                    case DIF_QUIT:
                        return STATE_SELECT_MAP;
                    default :
                        return STATE_MAIN_MENU;
                }
            }
            default :
                break;
        }
    }
    displayMenu();
    return STATE_SELECT_DIFFICULTY;
}

State maintainGameState(ALLEGRO_EVENT *event) {
    bool gameInProgress = true;
    if (event->keyboard.keycode == ALLEGRO_KEY_ESCAPE && event->type == ALLEGRO_EVENT_KEY_DOWN) {
        return STATE_GAME_PAUSE;
    } else if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN ||
               event->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
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
            case ALLEGRO_KEY_DOWN :
                setLowerOption(GAME_PAUSE_OPTIONS);
                break;
            case ALLEGRO_KEY_UP:
                setHigherOption(GAME_PAUSE_OPTIONS);
                break;
            case ALLEGRO_KEY_ESCAPE:
                return STATE_MAIN_MENU;
            case ALLEGRO_KEY_ENTER: {
                int option = getSelectedOption();
                switch (option) {
                    case GAME_PAUSE_CONTINUE:
                        return STATE_GAME;
                    case GAME_PAUSE_QUIT:
                    default :
                        return STATE_MAIN_MENU;
                }
            }
            default :
                break;
        }
    }
    displayMenu();
    return STATE_GAME_PAUSE;
}

State maintainGameFinishedState(ALLEGRO_EVENT *event) {
    static bool statsVisible = true;
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_LEFT :
                setLowerOption(GAME_FINISHED_OPTIONS);
                break;
            case ALLEGRO_KEY_RIGHT:
                setHigherOption(GAME_FINISHED_OPTIONS);
                break;
            case ALLEGRO_KEY_ESCAPE: {
                statsVisible = true;
                return STATE_MAIN_MENU;
            }
            case ALLEGRO_KEY_ENTER: {
                int option = getSelectedOption();
                statsVisible = true;
                switch (option) {
                    case GAME_FINISHED_SAVE:
                        return STATE_SAVE_STATS;
                    case GAME_PAUSE_QUIT:
                    default :
                        return STATE_MAIN_MENU;
                }
            }
            case ALLEGRO_KEY_SPACE:
                statsVisible = !statsVisible;
                break;
            default :
                break;
        }
    }
    if (event->type == ALLEGRO_EVENT_TIMER) {
        displayMap(true);
        if (statsVisible) {
            displayGameResult();
            displayGameFinishedMenu();
        }
    }
    return STATE_GAME_FINISHED;
}


State maintainShowStatsState(ALLEGRO_EVENT *event) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_DOWN :
                getHigherStat();
                break;
            case ALLEGRO_KEY_UP:
                getLowerStat();
                break;
            case ALLEGRO_KEY_ESCAPE:
                return STATE_MAIN_MENU;
            default :
                break;
        }
    }
    showStats();
    return STATE_SHOW_RANKING;
}

State maintainSaveResultState(ALLEGRO_EVENT *event) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_ESCAPE:
                return STATE_MAIN_MENU;
            case ALLEGRO_KEY_ENTER: {
                bool result = saveStat();
                if (result) {
                    cout << "saved" << endl;
                } else {
                    cout << "not saved" << endl;
                }
                return STATE_MAIN_MENU;
            }
            default : {
                updateLastPlayerName(event);
                break;
            }
        }
    }
    displayPlayerNameInput();
    return STATE_SAVE_STATS;
}

