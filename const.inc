LEVEL_UP_TRESHOLD = 10;
STARTING_LEVEL_MAX = 15;

TILE_SIZE = 10;                                  // NOWA stała
WELL_LEFT = 115;                                 // było 130, przesunięte bo studnia szersza
WELL_TOP = 6;                                   // było 34, podniesione
WELL_WIDTH = 10;      
WELL_HEIGHT = 19;                                // było 20
RCOL = WELL_LEFT+(WELL_WIDTH*TILE_SIZE)+10;      // było *8)+30
LCOL = 5;

JOY_DELAY = 5;
{$IFDEF HOSPES}
GAME_OVER_DELAY = 850;
{$ELSE}
GAME_OVER_DELAY = 385;
{$ENDIF}

CRACK_SOUND_LENGTH = 2;

PAUSE_SAVE_X = WELL_LEFT + 6;
PAUSE_SAVE_Y = WELL_TOP + 76;
PAUSE_SAVE_W = WELL_LEFT + (WELL_WIDTH * TILE_SIZE) - 7;
PAUSE_SAVE_H = WELL_TOP + 102;
PAUSE_BUF_SIZE = (PAUSE_SAVE_W div 2) * (PAUSE_SAVE_H + 1);

{$IFDEF HOSPES}
CLEAN_COLOR = 0;
{$ELSE}
CLEAN_COLOR = 2;
{$ENDIF}


KEY_ENTER = $1C; // Keyboard Return (ENTER)
KEY_ESC = $01; // Keyboard ESCAPE
KEY_SPACE = $39; // Keyboard Spacebar
KEY_RIGHT = $4D; // Keyboard Right Arrow
KEY_LEFT = $4B; // Keyboard Left Arrow
KEY_DOWN = $50; // Keyboard Down Arrow
KEY_UP = $48; // Keyboard Up Arrow
KEY_Y = $15;
KEY_N = $31;
KEY_P = $19;
KEY_M = $32;
KEY_PREVIOUS = $33;
KEY_NEXT = $34;

speed_table: array [1..15] of byte = (
    30,  // level 1
    28,  // level 2
    26,  // level 3
    24,  // level 4
    22,  // level 5
    20,  // level 6
    18,  // level 7
    16,  // level 8
    14,  // level 9
    12,  // level 10
    10,  // level 11
    8,   // level 12
    6,   // level 13
    4,   // level 14
    2    // level 15
);

ALL_BLACK: array [0..15] of word = (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
