#ifndef DEFS_HPP
#define DEFS_HPP

//CONST
#define GAME_TITLE "MINESWEEPER"
const int GAME_SCREEN_WIDTH = 889;
const int GAME_SCREEN_HEIGHT = 500;

const int TILE_SIZE = 28;
const int MAX_TILE_STATE = 12;
const int MAX_NUMBER_SPRITE = 10;
const int BOMB_TILE = 9;
const int UNPRESSED_TILE = 10;
const int FLAG_TILE = 11;
const int PRESSED_TILE = 0;

const int FONT_PTSIZE = 20;

const int START_SCENE_BUTTON_POSY = 400;
const int START_BUTTON_POSX = 250;
const int INSTRUCTION_BUTTON_POSX = 400;
const int EXIT_BUTTON_POSX = 600;

const int MODE_SCENE_BUTTON_POSY = 80;
const int EASYMODE_BUTTON_POSX = 70;
const int MEDIUMMODE_BUTTON_POSX = 370;
const int HARDMODE_BUTTON_POSX = 670;

const int MAX_SEC = 1000;

const int BACK_BUTTON_DEF = 50;
const int RESTART_BUTTON_DEF = 50;
const int SPEAKER_BUTTON_DEF = 50;

const int BOARD_ALL_MODE_POSY = 50;

const int DX = 21;
const int DY = 163;
const int NUMX = 25;
const int NUMY = 80;

const int EASYMODE_SCREEN_WIDTH = 294;
const int EASYMODE_SCREEN_HEIGHT = 436;
const int EASYMODE_BOARD_SIZE_X = 9;
const int EASYMODE_BOARD_SIZE_Y = 9;
const int EASYMODE_BOMBS_COUNT = 10;
const int EASYMODE_TIMENUMX = 235;

const int MEDIUMMODE_SCREEN_WIDTH = 488;
const int MEDIUMMODE_SCREEN_HEIGHT = 636;
const int MEDIUMMODE_BOARD_SIZE_X = 16;
const int MEDIUMMODE_BOARD_SIZE_Y = 16;
const int MEDIUMMODE_BOMBS_COUNT = 40;
const int MEDIUMMODE_TIMENUMX = 430;

const int HARDMODE_SCREEN_WIDTH = 880;
const int HARDMODE_SCREEN_HEIGHT = 632;
const int HARDMODE_BOARD_SIZE_X = 30;
const int HARDMODE_BOARD_SIZE_Y = 16;
const int HARDMODE_BOMBS_COUNT = 99;
const int HARDMODE_TIMENUMX = 820;


#endif // DEFS_HPP