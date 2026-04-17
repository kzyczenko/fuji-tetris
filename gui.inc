{$L graphics.o}

type
{$IFNDEF HOSPES}
  sGraphicPos = record
      mX : word;
      mY : word;
    end;
{$ENDIF}

  sGraphicRect = record
      mX : word;
      mY : word;
      mWidth : word;
      mHeight : word;
    end;

{$IFNDEF HOSPES}
  PsGraphicPos  = ^sGraphicPos;
{$ENDIF}
  PsGraphicRect  = ^sGraphicRect;

{$IFNDEF HOSPES}
procedure DrawPixel(screen: pointer; coords: PsGraphicPos; color: integer);external;
{$ENDIF}

procedure DrawBox(screen: pointer; box: PsGraphicRect; color: integer);external;

procedure SavePalette(palette: pword);
var
    i: smallint;
begin
    for i:=0 to 15 do
        palette[i]:=xbios_setcolor(i,-1);
end;

procedure SwapScreen;
var
    SCREEN_TMP: pword;
begin
    SCREEN_TMP := SCREEN_LOG;
    SCREEN_LOG := SCREEN_PHY;
    SCREEN_PHY := SCREEN_TMP;
    xbios_setscreen(SCREEN_LOG, SCREEN_PHY, -1);
    xbios_vsync;
end;

procedure ClearScreen(address: pword);
begin
    FillWord((address-640)^,16640,0);
end;

procedure FlashBackground;
var
    bg_color: array [0..11] of word = ($992, $22a, $aa3, $33b, $bb4, $44c, $cc5, $55d, $dd6, $66e, $ee7, $7ff);
    i: byte;
begin
    for i:=0 to 11 do begin
        xbios_setcolor(0, bg_color[i]);
        xbios_vsync;
    end;
    for i:=11 downto 0 do begin
        xbios_setcolor(0, bg_color[i]);
        xbios_vsync;
    end;
end;

procedure ShakeScreen;
begin
    SCREEN_LOG := SCREEN_LOG - 8*80;
    SwapScreen;
    SCREEN_PHY := SCREEN_PHY + 8*80;
    SwapScreen;
end;

procedure LineA_Init;
begin
    asm
      ; Save registers
      movem.l   D0-D2/A0-A2,-(A7)
      ; Line-A opcode
      dc.w      $A000
      ; Pointer parameter block LINEA
      move.l    A0,pParamblk
      ; Pointer system fonts
      move.l    A1,pFnthdr
      ; Pointer start addr. Line-A routines
      move.l    a2,pFktadr
      ; Restore registers
      movem.l   (a7)+,d0-d2/a0-a2
    end;
end;

procedure TextBlt(x, y, fg_color, bg_color, mode: word; ch: char);
begin
    asm
        movem.l   D0-D2/A0-A2,-(A7)
        move.l    pParamblk,A0
        move.l    pFnthdr,A1
        move.l    4(A1),A1
        move.l    76(A1),84(A0)
        move.w    80(A1),88(A0)
        move.w    82(A1),82(A0)
        move.w    mode,36(A0)
        move.w    #0,54(A0)
        move.w    #0,56(A0)
        move.w    #0,58(A0)
        move.w    #126,60(A0)
        move.w    #200,62(A0)
        move.w    #$8000,64(A0)
        move.w    #1,70(A0)
        move.l    72(A1),A3
        clr.l     D0
        move.b    ch,D0
        sub.w     36(A1),D0
        lsl.w     #1,D0
        move.w    0(A3,D0),72(A0)
        move.w    2(A3,D0),D0
        sub.w     72(A0),D0
        move.w    D0,80(A0)
        move.w    #0,74(A0)
        move.w    x,76(A0)
        move.w    y,78(A0)
        move.w    #0,90(A0)
        move.w    #0,102(A0)
        move.w    #0,104(A0)
        move.w    fg_color,106(A0)
        move.w    bg_color,114(A0)
        dc.w      $A008
        movem.l   (A7)+,D0-D2/A0-A2
    end;
end;

procedure NeoDrawString(x, y, color: word; apString: string);
var
    i: word;
begin
    for i := 1 to Length(apString) do
    begin
        TextBlt(x, y, color, 0, 1, apString[i]);
        x:=x+6;
    end;
end;

{$IFNDEF HOSPES}
procedure NeoDrawPixel(x,y,color:integer);
var
    coords: sGraphicPos;
begin
    coords.mX := x;
    coords.mY := y;
    DrawPixel(SCREEN_LOG, @coords, color);
end;
{$ENDIF}

procedure NeoDrawRect(x1,y1,x2,y2,color:word);
var
    box: sGraphicRect;
begin
    box.mX := x1;
    box.mY := y1;
    box.mWidth := x2-x1+1;
    box.mHeight := y2-y1+1;
    DrawBox(SCREEN_LOG, @box, color);
end;

procedure NeoDrawRectPhy(x1,y1,x2,y2,color:word);
var
    box: sGraphicRect;
begin
    box.mX := x1;
    box.mY := y1;
    box.mWidth := x2-x1+1;
    box.mHeight := y2-y1+1;
    DrawBox(SCREEN_PHY, @box, color);
end;

function ActiveScreenIndex: byte;
begin
    if SCREEN_LOG = SCREEN_0 then
        ActiveScreenIndex := 0
    else
        ActiveScreenIndex := 1;
end;

procedure ResetHudCache;
begin
    countersValid[0] := false;
    countersValid[1] := false;
    statsValid[0] := false;
    statsValid[1] := false;
end;

procedure DrawCounters;
var
    screenIndex: byte;
begin
    screenIndex := ActiveScreenIndex;

    if (not countersValid[screenIndex]) or (shownTopscore[screenIndex] <> topscore) then begin
        NeoDrawRect(LCOL+46,WELL_TOP+22,LCOL+46+(8*6),WELL_TOP+28,CLEAN_COLOR);
        s:=GetPaddedInt(topscore,8);
        NeoDrawString(LCOL+45,WELL_TOP+22,8,s);
        shownTopscore[screenIndex] := topscore;
    end;

    if (not countersValid[screenIndex]) or (shownLevel[screenIndex] <> level) then begin
        NeoDrawRect(LCOL+80,WELL_TOP+12,LCOL+80+(6*2),WELL_TOP+18,CLEAN_COLOR);
        s:=GetPaddedInt(level,2);
        NeoDrawString(LCOL+80,WELL_TOP+12,8,s);
        shownLevel[screenIndex] := level;
    end;

    if (not countersValid[screenIndex]) or (shownScore[screenIndex] <> score) then begin
        NeoDrawRect(LCOL+46,WELL_TOP+32,LCOL+46+(8*6),WELL_TOP+38,CLEAN_COLOR);
        s:=GetPaddedInt(score,8);
        NeoDrawString(LCOL+45,WELL_TOP+32,8,s);
        shownScore[screenIndex] := score;
    end;

    if (not countersValid[screenIndex]) or (shownTotalLines[screenIndex] <> totalLines) then begin
        NeoDrawRect(LCOL+68,WELL_TOP+2,LCOL+68+(6*4),WELL_TOP+8,CLEAN_COLOR);
        s:=GetPaddedInt(totalLines,4);
        NeoDrawString(LCOL+68,WELL_TOP+2,8,s);
        shownTotalLines[screenIndex] := totalLines;
    end;

    countersValid[screenIndex] := true;
end;

procedure DrawSmallSquare(x,y: word; tiletype: byte);
begin
    NeoDrawRect(x,y,x+2,y+2,tiletype+8);
end;

procedure DrawSquareNext(x,y,tiletype,xoff,yoff:byte);overload;
var tx,ty:word;
begin
    tx:=(x * TILE_SIZE) + LCOL - xoff;
    ty:=(y * TILE_SIZE) + WELL_TOP - yoff + 4;
    NeoDrawRect(tx-5, ty, tx+TILE_SIZE-6, ty+TILE_SIZE-1, tiletype+8);
    NeoDrawRect(tx-5, ty, tx+TILE_SIZE-7, ty, 6);
    NeoDrawRect(tx-5, ty, tx-5, ty+TILE_SIZE-2, 6);
    NeoDrawRect(tx+TILE_SIZE-6, ty, tx+TILE_SIZE-6, ty+TILE_SIZE-1, 2);
    NeoDrawRect(tx-5, ty+TILE_SIZE-1, tx+TILE_SIZE-6, ty+TILE_SIZE-1, 2);
end;

procedure DrawSquare(x,y,tiletype,xoff,yoff:byte);overload;
var tx,ty:word;
begin
    tx:=(x * TILE_SIZE) + WELL_LEFT - xoff;
    ty:=(y * TILE_SIZE) + WELL_TOP - yoff;
    NeoDrawRect(tx, ty, tx+TILE_SIZE-1, ty+TILE_SIZE-1, tiletype+8);
    NeoDrawRect(tx, ty, tx+TILE_SIZE-2, ty, 6);
    NeoDrawRect(tx, ty, tx, ty+TILE_SIZE-2, 6);
    NeoDrawRect(tx+TILE_SIZE-1, ty, tx+TILE_SIZE-1, ty+TILE_SIZE-1, 2);
    NeoDrawRect(tx, ty+TILE_SIZE-1, tx+TILE_SIZE-1, ty+TILE_SIZE-1, 2);
end;

procedure DrawSquare(x,y,tiletype:byte);overload;
begin
    DrawSquare(x,y,tiletype,0,0);
end;

procedure ClearSquare(x,y:byte);
var tx,ty:word;
begin
    tx:=x * TILE_SIZE + WELL_LEFT;
    ty:=y * TILE_SIZE + WELL_TOP;
    NeoDrawRect(tx, ty, tx+TILE_SIZE-1, ty+TILE_SIZE-1, 2);
end;

procedure ClearSquarePhy(x,y:byte);
var tx,ty:word;
begin
    tx:=x * TILE_SIZE + WELL_LEFT;
    ty:=y * TILE_SIZE + WELL_TOP;
    NeoDrawRectPhy(tx, ty, tx+TILE_SIZE-1, ty+TILE_SIZE-1, 2);
end;

procedure ClearNext;
begin
    NeoDrawRect(LCOL+20,WELL_TOP+68,LCOL+75,WELL_TOP+90,CLEAN_COLOR);
end;

procedure DrawNext(xp,yp:byte);
var 
    x,y,i,cell:byte;
begin
    for i := 0 to 3 do begin
        cell := tile_cells[0,nextTile,i];
        x := cell and $0f;
        y := cell shr 4;
        DrawSquareNext(xp+x,yp+y,nextTile+1,nextXoff[nextTile],nextYoff[nextTile]);
    end;
end;

{$IFDEF HOSPES}
procedure Box(x0,y0,x1,y1:word;c1,c2:byte);
begin
    // NeoDrawRect(x0,y0,x1,y1,2);
    NeoDrawRect(x0-2, y0-2, x1+4, y0-2, c1);
    NeoDrawRect(x1+4, y0-2, x1+4, y1+4, c1);
    NeoDrawRect(x0-2, y1+4, x1+4, y1+4, c1);
    NeoDrawRect(x0-2, y0-2, x0-2, y1+4, c1);
    NeoDrawRect(x0-4, y0-4, x1+2, y0-4, c2);
    NeoDrawRect(x1+2, y0-4, x1+2, y1+2, c2);
    NeoDrawRect(x0-4, y1+2, x1+2, y1+2, c2);
    NeoDrawRect(x0-4, y0-4, x0-4, y1+2, c2);
end;

procedure WellBox(x0,y0,x1,y1:word;c1,c2:byte);
begin
    NeoDrawRect(x0+1, y0+1, x1+3, y0+1, c1);
    NeoDrawRect(x1+3, y0+1, x1+3, y1+3, c1);
    NeoDrawRect(x0+1, y1+3, x1+3, y1+3, c1);
    NeoDrawRect(x0+1, y0+1, x0+1, y1+3, c1);
    NeoDrawRect(x0-1, y0-1, x1+1, y0-1, c2);
    NeoDrawRect(x1+1, y0-1, x1+1, y1+1, c2);
    NeoDrawRect(x0-1, y1+1, x1+1, y1+1, c2);
    NeoDrawRect(x0-1, y0-1, x0-1, y1+1, c2);
    NeoDrawRect(x0,y0,x1,y1,2);
end;

{$ELSE}
procedure DrawCorners(x0,y0,x1,y1,c:word);
begin
    NeoDrawPixel(x0,y0,c);
    NeoDrawPixel(x0,y1,c);
    NeoDrawPixel(x1,y0,c);
    NeoDrawPixel(x1,y1,c);
end;

procedure Box(x0,y0,x1,y1:word;c1,c2:byte);
begin
    NeoDrawRect(x0-3,y0-3,x1+3,y1+3,c2);
    NeoDrawRect(x0-2,y0-2,x1+2,y1+2,c1);
    NeoDrawRect(x0-1,y0-1,x1+1,y1+1,0);
    DrawCorners(x0-2,y0-2,x1+2,y1+2,0);
    DrawCorners(x0-3,y0-3,x1+3,y1+3,0);
    NeoDrawRect(x0,y0,x1,y1,2);
end;
{$ENDIF}

procedure ShowModal(s:string);
begin
    NeoDrawRect(WELL_LEFT+6,WELL_TOP+78,WELL_LEFT+(WELL_WIDTH*TILE_SIZE)-7,WELL_TOP+92,0);
    Box(WELL_LEFT+8,WELL_TOP+80,WELL_LEFT+(WELL_WIDTH*TILE_SIZE)-9,WELL_TOP+90,4,4);
    NeoDrawString(WELL_LEFT+9,WELL_TOP+82,8,s);
end;

procedure SaveBackground(x,y,w,h:word;dst:pbyte);
var py:word;
    offset:word;
    screen:pbyte;
begin
    screen := PByte(SCREEN_LOG);
    offset := y * 160;
    offset := offset + (x shr 4) shl 3;
    w := Trunc(w/2);

    for py := 0 to h do begin
        move(screen[offset], dst^, w);
        dst := dst + w;
        offset := offset + 160;
    end;
end;

procedure DrawImage(x,y,w,h:word;src:pbyte);
var py:word;
    offset:word;
    screen:pbyte;
begin
    screen := PByte(SCREEN_LOG);
    offset := y * 160;
    offset := offset + (x shr 4) shl 3;
    w := Trunc(w/2);
    // offset := offset + (x shr 4) shl 3;

    for py := 0 to h-1 do begin
        move(src^, screen[offset], w);
        src := src + w;
        offset := offset + 160;
    end;
end;

procedure DrawSmallTile(xp, yp: word; tiletype: byte);
var 
    x,y,i,cell:byte;
begin
    for i := 0 to 3 do begin
        cell := tile_cells[0,tiletype,i];
        x := cell and $0f;
        y := cell shr 4;
        DrawSmallSquare(xp+(x*2),yp+(y*2),tiletype+1);
    end;
end;

procedure DrawStatTiles;
var
    tiletype, y: byte;
begin
    y := 0;
    for tiletype := 0 to 6 do begin
        DrawSmallTile(LCOL+5, WELL_TOP+123+y, tiletype);
        y := y + 8;
    end;
end;

procedure DrawStats;
var
    screenIndex, tiletype, y: byte;
begin
    screenIndex := ActiveScreenIndex;
    y := 0;
    for tiletype := 0 to 6 do begin
        if (not statsValid[screenIndex]) or (shownStats[screenIndex,tiletype] <> stats[tiletype]) then begin
            NeoDrawRect(LCOL+20,WELL_TOP+123+y,LCOL+95,WELL_TOP+129+y,CLEAN_COLOR);
            str(stats[tiletype], s);
            NeoDrawString(LCOL+90-Length(s)*6,WELL_TOP+123+y,8,s);
            shownStats[screenIndex,tiletype] := stats[tiletype];
        end;
        y := y + 8;
    end;
    statsValid[screenIndex] := true;
end;

procedure DrawHUD;
begin
    DrawNext(5,7);
    DrawStats;
end;

procedure DrawBackground;
begin
    // ClrScr;
    // NeoDrawRect(0,0,320,200);
    DrawImage(RCOL+10,10,96,102,@LOGO);
    s:='FIRE - rotate';
    NeoDrawString(RCOL+10,140,4,s);
    s:=chr(1)+' - drop hard';
    NeoDrawString(RCOL+10,150,4,s);
    s:=chr(2)+' - drop soft';
    NeoDrawString(RCOL+10,160,4,s);
    s:=chr(4)+' - move left';
    NeoDrawString(RCOL+10,170,4,s);
    s:=chr(3)+' - move right';
    NeoDrawString(RCOL+10,180,4,s);
    s:='ESC - quit';
    NeoDrawString(RCOL+10,190,4,s);
end;

procedure DrawGui;
begin
{$IFDEF HOSPES}
    WellBox(WELL_LEFT,WELL_TOP,WELL_LEFT+(WELL_WIDTH*TILE_SIZE)-1,WELL_TOP+(WELL_HEIGHT*TILE_SIZE)-1,4,4);
{$ELSE}
    Box(WELL_LEFT,WELL_TOP,WELL_LEFT+(WELL_WIDTH*TILE_SIZE)-1,WELL_TOP+(WELL_HEIGHT*TILE_SIZE)-1,4,4);
{$ENDIF}
    Box(LCOL,WELL_TOP,LCOL+95,WELL_TOP+40,2,4); // LINES
    
    // Box(LCOL,WELL_TOP+20,LCOL+95,WELL_TOP+40,2,4); // SCORExTOP
    Box(LCOL+20,WELL_TOP+56,LCOL+75,WELL_TOP+90,4,4); // NEXT
    // Box(LCOL+15,WELL_TOP+96,LCOL+80,WELL_TOP+106,2,4); // LEVEL
    Box(LCOL,WELL_TOP+106,LCOL+95,WELL_TOP+(WELL_HEIGHT*TILE_SIZE)-1,2,4); // STATISTICS

    DrawStatTiles;

    SetLength(s,0);

    s := 'Lines:' ;
    NeoDrawString(LCOL+1,WELL_TOP+2,8,s);
    s:='Level:';
    NeoDrawString(LCOL+1,WELL_TOP+12,8,s);
    s := 'Top:' ;
    NeoDrawString(LCOL+1,WELL_TOP+22,8,s);
    s:='Score:';
    NeoDrawString(LCOL+1,WELL_TOP+32,8,s);
    s:='NEXT';
    NeoDrawString(LCOL+35,WELL_TOP+58,8,s);
    s:='Statistics';
    NeoDrawString(LCOL+10,WELL_TOP+110,8,s);
    DrawStats;
    DrawCounters;
end;

procedure UpdateStartingLevel(col:byte);
begin
    s := GetPaddedInt(startingLevel,2);
    s := Concat('Starting level: ',s);
    NeoDrawRect(129,160,142,166,0);
    NeoDrawString(33,160,col,s);
end;

function KeyPressed:boolean;
var
    key: shortint;
begin
    key:=bios_bconstat(2);
    if key=0 then
        KeyPressed:=false
    else
        KeyPressed:=true
end;

procedure ReadJoy(var moveKey, actionKey: byte);
var b: byte;
begin
    moveKey := 0;
    actionKey := 0;
    b := joyStatus;
    if b and 4 <> 0 then moveKey := KEY_LEFT;
    if b and 8 <> 0 then moveKey := KEY_RIGHT;
    if b and 1 <> 0 then moveKey := KEY_UP;
    if b and 2 <> 0 then moveKey := KEY_DOWN;
    if b and 128 <> 0 then actionKey := KEY_ENTER;   // FIRE = rotacja
end;

function ReadKeyboard:byte;
var
    key: longint;
begin
    key := bios_bconin(2);
    ReadKeyboard := byte(key shr 16)
end;

procedure GetUserInput;
var
    currentFireState: byte;
begin
    key := 0;
    actionKey := 0;

    if (KeyPressed) then begin
        key := ReadKeyboard();
        // Klawiatura: SPACE/ENTER to akcja, reszta to ruch
        if (key = KEY_SPACE) or (key = KEY_ENTER) then begin
            actionKey := key;
            key := 0;
        end;
    end;

    if (key = 0) and (actionKey = 0) then
        ReadJoy(key, actionKey);

    // Edge detection dla FIRE (rotacji z joysticka)
    // Rotacja działa tylko na WCIŚNIĘCIE, nie na TRZYMANIE
    currentFireState := actionKey;
    if (actionKey <> 0) and (prevFireState <> 0) then
        actionKey := 0;  // FIRE było już wciśnięte — ignoruj
    prevFireState := currentFireState;

    // joyDelay dotyczy TYLKO ruchu, nie rotacji
    if key = 0 then begin
        joyDelay := 0;
    end;

    if joyDelay = 0 then begin 
        if key <> 0 then
            joyDelay := JOY_DELAY;
    end else begin  
        Dec(joyDelay);
        key := 0;
    end;
end;

procedure QuitGame;
begin
    ShowModal(' QUIT (Y/N)?');
    SwapScreen;  
    repeat 
        GetUserInput;
        if key = KEY_Y then begin
            quit := true;
        end;
        if (key = KEY_N) or (key = KEY_ESC)then begin
            quit := false;
            Move(SCREEN_LOG^, SCREEN_PHY^, 32000);
        end;
    until (key = KEY_Y) or (key = KEY_N) or (key = KEY_ESC);
end;

procedure PauseGame;
begin
    SaveBackground(PAUSE_SAVE_X, PAUSE_SAVE_Y, PAUSE_SAVE_W, PAUSE_SAVE_H, @pauseBuffer[0]);
    ShowModal(' GAME PAUSED');
    SwapScreen;
    repeat 
        GetUserInput;
    until (key=KEY_P) or (key=KEY_ESC);
    SwapScreen;
    DrawImage(PAUSE_SAVE_X, PAUSE_SAVE_Y, PAUSE_SAVE_W, PAUSE_SAVE_H, @pauseBuffer[0]);
end;

procedure LogoScreen;
begin
    ClearScreen(SCREEN_PHY);
    xbios_setpalette(@ALL_BLACK);
    UnApl(@TEAMLOGO, SCREEN_LOG);
    xbios_setpalette(@TEAMLOGO_PAL);
    SwapScreen;
    Pause(120);
end;

procedure TitleScreen;
var f:byte;
begin
    ClearScreen(SCREEN_PHY);
    xbios_setpalette(@ALL_BLACK);
    UnApl(@INTRO, SCREEN_LOG);
    xbios_setpalette(@INTRO_PAL);
    NeoDrawString(170,160,1,'Code: bocianu/KtZ');
    NeoDrawString(170,170,1,' GFX: bocianu/Hospes');
{$IFDEF HOSPES}
    NeoDrawString(170,180,1,' MSX: TAO');
{$ELSE}
    NeoDrawString(170,180,1,' MSX: Dma-Sc');
{$ENDIF}
    Move(SCREEN_LOG^, SCREEN_PHY^, 32000);
    f:=0;
    repeat 
        UpdateStartingLevel(3);
        NeoDrawString(30,180,f,'Press FIRE to Start');
        GetUserInput;
        if key = KEY_LEFT then begin
            if startingLevel=1 then startingLevel:=STARTING_LEVEL_MAX else dec(startingLevel);
        end;
        if key = KEY_RIGHT then begin
            if startingLevel=STARTING_LEVEL_MAX then startingLevel:=1 else inc(startingLevel);
        end;
        if key = KEY_ESC then QuitGame;
        inc(f);
        if f > 5 then f := 0;
        SwapScreen;
    until (actionKey=KEY_ENTER) or (actionKey=KEY_SPACE) or quit;
    // UpdateStartingLevel(24);

    level := startingLevel;
    lines2levelup := level * LEVEL_UP_TRESHOLD;
end;
