//
// Created by witek on 02.03.17.
//

#ifndef MINESWEEPER_GAMESTATES_H
#define MINESWEEPER_GAMESTATES_H

#include <allegro5/events.h>
#include <allegro5/allegro.h>

enum State {
    STATE_SELECT_MAP, STATE_GAME, STATE_GAME_PAUSE, STATE_MAIN_MENU, STATE_EDITOR_MENU,
    STATE_EDITOR, STATE_EDITOR_PAUSE, STATE_SELECT_DIFFICULTY, STATE_SAVE_MAP, STATE_GAME_FINISHED,
    STATE_SAVE_STATS, STATE_SHOW_RANKING
};
enum GameState {
    GAME_STATE_PLAYING, GAME_STATE_WON, GAME_STATE_LOST
};
extern bool gameOver;

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

State maintainShowStatsState(ALLEGRO_EVENT *event);

State maintainSaveResultState(ALLEGRO_EVENT *event);

#endif //MINESWEEPER_GAMESTATES_H
