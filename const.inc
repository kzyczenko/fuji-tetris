VER = '0.96';

LEVEL_UP_TRESHOLD = 10;
STARTING_LEVEL_MAX = 15;

TILE_SIZE = 10;                                  // NOWA stała
WELL_LEFT = 110;                                 // było 130, przesunięte bo studnia szersza
WELL_TOP = 14;                                   // było 34, podniesione
WELL_WIDTH = 10;      
WELL_HEIGHT = 18;                                // było 20
RCOL = WELL_LEFT+(WELL_WIDTH*TILE_SIZE)+10;      // było *8)+30
LCOL = WELL_LEFT-30;

JOY_DELAY = 5;
{$IFDEF HOSPES}
GAME_OVER_DELAY = 850;
{$ELSE}
GAME_OVER_DELAY = 385;
{$ENDIF}

CRACK_SOUND_LENGTH = 2;

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
    50,  // level 1   = 1.00s
    46,  // level 2   = 0.92s
    42,  // level 3   = 0.84s
    38,  // level 4   = 0.76s
    34,  // level 5   = 0.68s
    29,  // level 6   = 0.58s
    24,  // level 7   = 0.48s
    19,  // level 8   = 0.38s
    15,  // level 9   = 0.30s
    12,  // level 10  = 0.24s
    9,   // level 11  = 0.18s
    7,   // level 12  = 0.14s
    5,   // level 13  = 0.10s
    3,   // level 14  = 0.06s
    2    // level 15  = 0.04s
);