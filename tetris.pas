{$APPTYPE GUI}

program tetris;
uses xbios, gemdos, bios;

const 
{$i const.inc}
{$i tiles.inc}
{$i palette.inc}
{$i images/logo.inc}
{$i images/intro.inc}
{$IFDEF HOSPES}
{$i msx/hmusici.inc}
{$i msx/hmusic1.inc}
{$i msx/hmusic2.inc}
{$i msx/hmusic3.inc}
{$i msx/hmusice.inc}
{$ELSE}
{$i msx/musici.inc}
{$i msx/music1.inc}
{$i msx/music2.inc}
{$i msx/music3.inc}
{$i msx/music4.inc}
{$i msx/music5.inc}
{$i msx/music6.inc}
{$i msx/musice.inc}
{$ENDIF}
var
    board: array[0..WELL_WIDTH-1,0..WELL_HEIGHT-1] of byte;
    score,topscore,previous_topscore: cardinal;
    lines2levelup: byte;
    levelLines: byte;
    totalLines: word;
    level: byte;
    startingLevel: byte;
    nextTile,currentTile,prevTile1,prevTile2: byte;
    rotation,prevRotation1,prevRotation2: byte;
    tileX,tileY,prevX1,prevX2,prevY1,prevY2: byte;
    key: byte;
    fallcounter: byte;
    joydelay: byte;
    scoretable: array [0..3] of word = (40,100,300,1200);
    tilepool: array [0..6] of word;
    poolsize: byte;
    s: string;
    spawnTile: boolean;
    comboCounter:byte;
    crackCounter:byte;
    joyStatus: byte;
    oldPalette: array[0..16] of word;
    oldRez: word;
    SCREEN_LOG, SCREEN_PHY, oldScreen: pword;
    pParamblk, pFnthdr, pFktadr: pword;
    quit: boolean;
 	vector_table: PKBDVECS;
	old_joyvec: pointer;
    joy_on:byte = $14;
    mouse_on:byte = $08;
    music_on: boolean = true;
    next_tune_counter: byte;
{$IFDEF HOSPES}
    game_tunes: array[0..3] of pointer = (@MUSIC3, @MUSIC1, @MUSIC2, @MUSICI);
{$ELSE}
    game_tunes: array[0..5] of pointer = (@MUSIC1, @MUSIC2, @MUSIC3, @MUSIC4, @MUSIC5, @MUSIC6);
{$ENDIF}
    current_tune: byte;
    MUSIC: pointer;
    start_music: pointer;
    music_length: dword;
    stats: array [0..6] of word = (0, 0, 0, 0, 0, 0, 0);
    counter: word;

{$i random.inc}
{$i helpers.inc}
{$i gui.inc}
{$i board.inc}

{$L sndhisr.o}

procedure SNDH_PlayTuneISR(sndh: pointer; tune: word);external;

procedure SNDH_StopTuneISR;external;

procedure ToggleMusic;
begin
    if music_on then begin
        music_on := false;
        SNDH_StopTuneISR;
    end else begin
        music_on := true;
        SNDH_PlayTuneISR(MUSIC, 1);
    end;
end;

procedure NextTune;
begin
    Inc(current_tune);
    if current_tune > Length(game_tunes)-1 then current_tune := 0;
    SNDH_StopTuneISR;
    UnApl(game_tunes[current_tune], MUSIC);
    SNDH_PlayTuneISR(MUSIC, 1);
end;

procedure PreviousTune;
begin
    Dec(current_tune);
    if current_tune = 255 then current_tune := Length(game_tunes) - 1;
    SNDH_StopTuneISR;
    UnApl(game_tunes[current_tune], MUSIC);  
    SNDH_PlayTuneISR(MUSIC, 1);
end;

procedure ReadJoyStatus(); assembler;
asm
    move.b 2(A0), joyStatus
    rts
end;

procedure InstallJoy;
begin
    xbios_ikbdws(0, @joy_on);
    vector_table := xbios_kbdvbase();
    old_joyvec := vector_table^.joyvec;
    vector_table^.joyvec := @ReadJoyStatus;
end;

procedure UnInstallJoy;
begin
    xbios_ikbdws(0, @mouse_on);
    vector_table := xbios_kbdvbase();
    vector_table^.joyvec := @old_joyvec;
end;

procedure ScreenInit(mode: smallint);
begin
    gemdos_super(pointer(0));
    oldRez := xbios_getrez;
    SavePalette(@oldPalette[0]);
    oldScreen := xbios_logbase;
    SCREEN_LOG := pointer(cardinal(gemdos_malloc(33280+256)) and $FFFFFF00 + 1280);
    SCREEN_PHY := pointer(cardinal(gemdos_malloc(33280+256)) and $FFFFFF00 + 1280);
    ClearScreen(SCREEN_LOG);
    ClearScreen(SCREEN_PHY);
    xbios_setscreen(SCREEN_LOG, SCREEN_PHY, mode);
    LineA_Init;
    InstallJoy;
end;

procedure Done;
begin
    UnInstallJoy;
    xbios_setscreen(oldScreen, oldScreen, oldRez);
    xbios_setpalette(@oldPalette);
    gemdos_mfree(SCREEN_LOG);
    gemdos_mfree(SCREEN_PHY);
    gemdos_super(pointer(1));
end;

procedure InitNewGame;
var
    i: byte;
begin
    score := 0;
    levelLines := 0;
    totalLines := 0;
    ClrBoard;
    DrawCounters;
    poolsize := 0;
    nextTile := TossTile;
    spawnTile := true;
    comboCounter := 0;
    crackCounter := 0;
    for i :=0 to 6 do stats[i] := 0;
    next_tune_counter := 1;
end;

begin
    MUSIC := gemdos_malloc(30000);
    ScreenInit(0);
    current_tune := 0;
    quit := false;
    startingLevel :=1 ;
    level := 1;
    joyDelay := 0;
    Randomize;
    LoadTopScore;

    repeat

        // title screen
        UnApl(@MUSICI, MUSIC);  
        SNDH_PlayTuneISR(MUSIC, 1);
        TitleScreen;
        ClearScreen(SCREEN_LOG);
        SwapScreen;
        LoadPalette;
        ClearScreen(SCREEN_LOG);
        SNDH_StopTuneISR;
      if not quit then begin  
        UnApl(game_tunes[current_tune], MUSIC);  
        SNDH_PlayTuneISR(MUSIC, 1);
        InitNewGame;
        DrawBackground;
        DrawGui;
        SwapScreen;
        DrawBackground;
        DrawGui;
        // main game loop
        repeat
          
            // new tile
            if spawnTile then
                begin
                    ClearNext;
                    SwapScreen;
                    ClearNext;
                    rotation := 0;
                    currentTile := nextTile;
                    prevTile1 := currentTile;
                    prevTile2 := currentTile;
                    tileX := (WELL_WIDTH - tiles_sizes[currentTile]) shr 1;
                    tileY := 0;
                    prevRotation1 := rotation;
                    prevRotation2 := rotation;
                    if currentTile = 1 then dec(tileY);
                    prevX1 := tileX;
                    prevY1 := tileY;
                    prevX2 := tileX;
                    prevY2 := tileY;
                    nextTile := TossTile;
                    DrawNext(18,6);
                    DrawStats;
                    SwapScreen;
                    DrawNext(18,6);
                    DrawStats;
                    spawnTile:=false;
                end;

            // this formula controls game speed based on players level
            fallcounter := 65-(Min(level,15)*4);

            // main steering loop
            repeat
                DrawBlock(prevX1,prevY1,prevTile1,prevRotation1,0);
                DrawBlock(prevX2,prevY2,prevTile2,prevRotation2,0);
                DrawBlock(tileX,tileY,currentTile,rotation,1);
                SwapScreen;
                DrawBlock(prevX1,prevY1,prevTile1,prevRotation1,0);
                DrawBlock(prevX2,prevY2,prevTile2,prevRotation2,0);
                DrawBlock(tileX,tileY,currentTile,rotation,1);
                dec(fallcounter);
                GetUserInput;
                if key<>0 then
                    begin
                        prevX1 := tileX;
                        prevY1 := tileY;
                        prevRotation1 := rotation;
                        prevTile1 := currentTile;
                        if (key=KEY_LEFT) and CanMoveBlock(tileX-1,tileY,currentTile,rotation) then tileX := tileX-1;
                        if (key=KEY_RIGHT) and CanMoveBlock(tileX+1,tileY,currentTile,rotation) then tileX := tileX+1;
                        if (key=KEY_SPACE) and CanMoveBlock(tileX,tileY,currentTile,TPred(rotation)) then rotation := TPred(rotation);
                        if (key=KEY_ENTER) and CanMoveBlock(tileX,tileY,currentTile,TSucc(rotation)) then rotation := TSucc(rotation);
                        if (key=KEY_UP) then FastFall; // fall to bottom
                        if (key=KEY_DOWN) then fallcounter := 0; // fall one row
                        if (key=KEY_P) then PauseGame; // Pause
                        if (key=KEY_M) then ToggleMusic; // Music on/off
                        if (key=KEY_NEXT) and music_on then NextTune;
                        if (key=KEY_PREVIOUS) and music_on then PreviousTune;
                    end;
                if crackCounter>0 then begin
                    dec(crackCounter);
                end;
            until (fallcounter=0) or (key=KEY_ESC);

            // clear old position
            prevX2 := tileX;
            prevY2 := tileY;
            prevRotation2 := rotation;
            prevTile2 := currentTile;

            // check the floor
            if CanMoveBlock(tileX,byte(tileY+1),currentTile,rotation) then
                tileY := byte(tileY+1)
            else
                begin // tile hits the ground
                    crackCounter := CRACK_SOUND_LENGTH;
                    DrawBlock(tileX,tileY,currentTile,rotation,1);
                    DrawBlock(tileX,tileY,currentTile,rotation,2);
                    SwapScreen;
                    DrawBlock(tileX,tileY,currentTile,rotation,1);
                    DrawBlock(tileX,tileY,currentTile,rotation,2);
                    SwapScreen;
                    CheckRows(tileY);
                    if tileY=0 then key := KEY_ESC // game over
                        else spawnTile:=true;
                end;
        until (key = KEY_ESC);
        // game over
        if previous_topscore<topscore then
            begin
                previous_topscore := topscore;
                SaveTopScore;
            end;
        SNDH_StopTuneISR;
        UnApl(@MUSICE, MUSIC);  
        SNDH_PlayTuneISR(MUSIC,1);    
        DrawCounters;
        ShowModal('   GAME      OVER   ');
        SwapScreen;
        DrawCounters;
        ShowModal('   GAME      OVER   ');
        counter := 0;
        repeat
            GetUserInput;
            Pause(1);
            inc(counter);
        until (counter=GAME_OVER_DELAY) or (key=KEY_ENTER) or (key=KEY_SPACE) or (key=KEY_ESC);
        SNDH_StopTuneISR;
        key := 0;

      end;

    until quit;
    SNDH_StopTuneISR;
    Done();

end.
