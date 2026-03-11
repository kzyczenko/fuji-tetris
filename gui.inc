{$L graphics.o}

type
  sGraphicPos = record
      mX : word;
      mY : word;
    end;

  sGraphicRect = record
      mX : word;
      mY : word;
      mWidth : word;
      mHeight : word;
    end;

  PsGraphicPos  = ^sGraphicPos;
  PsGraphicRect  = ^sGraphicRect;

procedure DrawPixel(screen: pointer; coords: PsGraphicPos; color: integer);external;

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

procedure TextBlt(x, y, color: word; ch: char);
begin
    asm
        movem.l   D0-D2/A0-A2,-(A7)
        move.l    pParamblk,A0
        move.l    pFnthdr,A1
        move.l    4(A1),A1
        move.l    76(A1),84(A0)
        move.w    80(A1),88(A0)
        move.w    82(A1),82(A0)
        move.w    #1,36(A0)
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
        move.w    color,106(A0)
        move.w    #0,114(A0)
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
        TextBlt(x, y, color, apString[i]);
        x:=x+6;
    end;
end;

procedure NeoDrawPixel(x,y,color:integer);
var
    coords: sGraphicPos;
begin
    coords.mX := x;
    coords.mY := y;
    DrawPixel(SCREEN_LOG, @coords, color);
end;

procedure NeoDrawLineH(x, y, w, color: integer);
var
    i, x1: integer;
begin
    i := x;
    repeat
        NeoDrawPixel(i, y, color);
        Inc(i);
    until i = x+w;
end;

procedure NeoDrawLineV(x, y, h, color: integer);
var
    i: integer;
begin
    i := y;
    repeat
        NeoDrawPixel(x, i, color);
        Inc(i);
    until i = y+h;
end;

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

procedure DrawCounters;
begin
    NeoDrawRect(RCOL+21,WELL_TOP-8,RCOL+21+(8*6),WELL_TOP-2,CLEAN_COLOR);
    NeoDrawRect(RCOL+21,WELL_TOP+12,RCOL+21+(8*6),WELL_TOP+18,CLEAN_COLOR);
    NeoDrawRect(RCOL+56,WELL_TOP+78,RCOL+56+(6*2),WELL_TOP+84,CLEAN_COLOR);
    NeoDrawRect(WELL_LEFT+54,WELL_TOP-18,WELL_LEFT+54+(6*4),WELL_TOP-12,CLEAN_COLOR);

    s:=GetPaddedInt(topscore,8);
    NeoDrawString(RCOL+20,WELL_TOP-8,8,s);

    s:=GetPaddedInt(score,8);
    NeoDrawString(RCOL+20,WELL_TOP+12,8,s);

    s:=GetPaddedInt(level,2);
    NeoDrawString(RCOL+56,WELL_TOP+78,8,s);

    s:=GetPaddedInt(totalLines,4);
    NeoDrawString(WELL_LEFT+54,WELL_TOP-18,8,s);
end;

procedure DrawSmallSquare(x,y: word; tiletype: byte);
begin
    NeoDrawRect(x,y,x+2,y+2,tiletype+8);
end;

procedure DrawSquare(x,y,tiletype,xoff,yoff:byte);overload;
var 
    tx,ty:word;
begin
    tx:=(x shl 3) + WELL_LEFT - xoff;
    ty:=(y shl 3) + WELL_TOP - yoff;
    // fill
    NeoDrawRect(tx,ty,tx+7,ty+7,tiletype+8);
    // highlight: użyj DrawBox zamiast pixel-po-pixelu
    NeoDrawRect(tx,ty,tx+6,ty,6);      // górna krawędź (1px wysoka)
    NeoDrawRect(tx,ty,tx,ty+6,6);      // lewa krawędź (1px szeroka)
    // shadow:
    NeoDrawRect(tx+7,ty,tx+7,ty+7,2);  // prawa krawędź
    NeoDrawRect(tx,ty+7,tx+7,ty+7,2);  // dolna krawędź
end;

procedure DrawSquare(x,y,tiletype:byte);overload;
begin
    DrawSquare(x,y,tiletype,0,0);
end;

procedure ClearSquare(x,y:byte);
var tx,ty:word;
begin
    tx:=x shl 3 + WELL_LEFT;
    ty:=y shl 3 + WELL_TOP;
    NeoDrawRect(tx,ty,tx+7,ty+7,2);
end;

procedure ClearSquarePhy(x,y:byte);
var tx,ty:word;
begin
    tx:=x shl 3 + WELL_LEFT;
    ty:=y shl 3 + WELL_TOP;
    NeoDrawRectPhy(tx,ty,tx+7,ty+7,2);
end;

procedure ClearNext;
begin
    NeoDrawRect(RCOL+15,WELL_TOP+42,RCOL+55,WELL_TOP+67,CLEAN_COLOR);
end;

procedure DrawNext(xp,yp:byte);
var 
    x,y,i:byte;
    tile: word;
    tilebody: pword;
begin
    i:=0;
    tilebody := getTile(nextTile,0);
    y:=0;
    repeat
        x:=0;
        repeat
            tile := (tilebody+i)^;
            if tile = 1 then 
                    DrawSquare(xp+x,yp+y,nextTile+1,nextXoff[nextTile],nextYoff[nextTile]);
            inc(i);
            inc(x);
        until x=tiles_sizes[nextTile];
        inc(y);
    until y=tiles_sizes[nextTile];
end;

procedure DrawCorners(x0,y0,x1,y1,c:word);
begin
    NeoDrawPixel(x0,y0,c);
    NeoDrawPixel(x0,y1,c);
    NeoDrawPixel(x1,y0,c);
    NeoDrawPixel(x1,y1,c);
end;

{$IFDEF HOSPES}
procedure Box(x0,y0,x1,y1:word;c1,c2:byte);
begin
    // NeoDrawRect(x0,y0,x1,y1,2);
    NeoDrawLine(x0-2, y0-2, x1+4, y0-2, 2);
    NeoDrawLine(x1+4, y0-2, x1+4, y1+4, 2);
    NeoDrawLine(x0-2, y1+4, x1+4, y1+4, 2);
    NeoDrawLine(x0-2, y0-2, x0-2, y1+4, 2);
    NeoDrawLine(x0-4, y0-4, x1+2, y0-4, c2);
    NeoDrawLine(x1+2, y0-4, x1+2, y1+2, c2);
    NeoDrawLine(x0-4, y1+2, x1+2, y1+2, c2);
    NeoDrawLine(x0-4, y0-4, x0-4, y1+2, c2);
end;

procedure WellBox(x0,y0,x1,y1:word;c1,c2:byte);
begin
    NeoDrawLine(x0+1, y0+1, x1+3, y0+1, 2);
    NeoDrawLine(x1+3, y0+1, x1+3, y1+3, 2);
    NeoDrawLine(x0+1, y1+3, x1+3, y1+3, 2);
    NeoDrawLine(x0+1, y0+1, x0+1, y1+3, 2);
    NeoDrawLine(x0-1, y0-1, x1+1, y0-1, c2);
    NeoDrawLine(x1+1, y0-1, x1+1, y1+1, c2);
    NeoDrawLine(x0-1, y1+1, x1+1, y1+1, c2);
    NeoDrawLine(x0-1, y0-1, x0-1, y1+1, c2);
    NeoDrawRect(x0,y0,x1,y1,2);
end;

{$ELSE}
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
var s2:string;
begin
    NeoDrawRect(WELL_LEFT+6,WELL_TOP+58,WELL_LEFT+(WELL_WIDTH*8)-7,WELL_TOP+82,0);
    Box(WELL_LEFT+8,WELL_TOP+60,WELL_LEFT+(WELL_WIDTH*8)-9,WELL_TOP+80,4,4);
    move(s[11],s2[1],10);
    SetLength(s,10);
    SetLength(s2,10);
    NeoDrawString(WELL_LEFT+9,WELL_TOP+62,8,s);
    NeoDrawString(WELL_LEFT+9,WELL_TOP+72,8,s2);
end;


procedure SaveBackground(x,y,w,h:word;dst:pbyte);
var px,py:word;
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
var px,py:word;
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
    x,y,i:byte;
    tile: word;
    tilebody: pword;
begin
    i:=0;
    tilebody := getTile(tiletype, 0);
    y:=0;
    repeat
        x:=0;
        repeat
            tile := (tilebody+i)^;
            if tile = 1 then 
                    DrawSmallSquare(xp+(x*2),yp+(y*2),tiletype+1);
            inc(i);
            inc(x);
        until x=tiles_sizes[tiletype];
        inc(y);
    until y=tiles_sizes[tiletype];
end;

procedure DrawStatTiles;
var
    tiletype, y: byte;
begin
    y := 0;
    for tiletype := 0 to 6 do begin
        DrawSmallTile(LCOL-60, WELL_TOP+103+y, tiletype);
        y := y + 8;
    end;
end;

procedure DrawStats;
var
    tiletype, y: byte;
begin
    y := 0;
    NeoDrawRect(LCOL-50,WELL_TOP+103,LCOL-10,WELL_TOP+159,CLEAN_COLOR);
    for tiletype := 0 to 6 do begin
        str(stats[tiletype], s);
        NeoDrawString(LCOL-10-Length(s)*6,WELL_TOP+103+y,8,s);
        y := y + 8;
    end;
end;

procedure DrawBackground;
begin
    // ClrScr;
    // NeoDrawRect(0,0,320,200);
    DrawImage(16,8,96,102,@LOGO);
    s:='FIRE - rotate';
    NeoDrawString(RCOL-10,140,4,s);
    s:=chr(1)+' - drop hard';
    NeoDrawString(RCOL-10,150,4,s);
    s:=chr(2)+' - drop soft';
    NeoDrawString(RCOL-10,160,4,s);
    s:=chr(4)+' - move left';
    NeoDrawString(RCOL-10,170,4,s);
    s:=chr(3)+' - move right';
    NeoDrawString(RCOL-10,180,4,s);
    s:='ESC - quit';
    NeoDrawString(RCOL-10,190,4,s);
end;

procedure DrawGui;
begin
{$IFDEF HOSPES}
    WellBox(WELL_LEFT,WELL_TOP,WELL_LEFT+(WELL_WIDTH*8)-1,WELL_TOP+(WELL_HEIGHT*8)-1,4,4);
{$ELSE}
    Box(WELL_LEFT,WELL_TOP,WELL_LEFT+(WELL_WIDTH*8)-1,WELL_TOP+(WELL_HEIGHT*8)-1,4,4);
{$ENDIF}
    Box(WELL_LEFT,WELL_TOP-20,WELL_LEFT+(WELL_WIDTH*8)-1,WELL_TOP-10,2,4);
    
    Box(RCOL,WELL_TOP-20,RCOL+70,WELL_TOP+20,2,4);
    Box(RCOL+15,WELL_TOP+30,RCOL+55,WELL_TOP+66,4,4);
    Box(RCOL,WELL_TOP+76,RCOL+70,WELL_TOP+86,2,4);
    Box(LCOL-70,WELL_TOP+86,LCOL,WELL_TOP+(WELL_HEIGHT*8)-1,2,4);
    DrawStatTiles;

    SetLength(s,0);

    s := 'Lines:' ;
    NeoDrawString(WELL_LEFT+1,WELL_TOP-18,8,s);
    s := 'Top:' ;
    NeoDrawString(RCOL+1,WELL_TOP-18,8,s);
    s:='Score:';
    NeoDrawString(RCOL+1,WELL_TOP+2,8,s);
    s:='NEXT';
    NeoDrawString(RCOL+22,WELL_TOP+32,8,s);
    s:='Level:';
    NeoDrawString(RCOL+1,WELL_TOP+78,8,s);
    s:='Statistics';
    NeoDrawString(LCOL-65,WELL_TOP+90,8,s);
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

function ReadJoy:byte;
var b:byte;
begin
    readJoy:=0;
    b := joyStatus;
    if b and 4 <> 0 then readJoy := KEY_LEFT; // left
    if b and 8 <> 0 then readJoy := KEY_RIGHT; // right
    if b and 1 <> 0 then readJoy := KEY_UP; // up
    if b and 2 <> 0 then readJoy := KEY_DOWN;; // down
    if b and 128 <> 0 then readJoy := KEY_ENTER; // butA
end;

function ReadKeyboard:byte;
var
    key: longint;
begin
    key := bios_bconin(2);
    ReadKeyboard := byte(key shr 16)
end;

procedure GetUserInput;
begin
    if (KeyPressed) then key := ReadKeyboard();
    if key = 0 then key := ReadJoy();
    if key = 0 then begin
        joyDelay := 0;
    end;

    if joyDelay = 0 then begin 
        if key <> 0 then begin
            joyDelay := JOY_DELAY;
        end;
    end else begin  
        Dec(joyDelay);
        key := 0;
    end;
end;

procedure QuitGame;
begin
    ShowModal('   Quit     (Y/N)?  ');
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
var
    dst: pbyte;
begin
    dst := gemdos_malloc((WELL_LEFT+(WELL_WIDTH*8)-7) * Trunc((WELL_TOP+(56-82)) / 2));
    SaveBackground(WELL_LEFT+6,WELL_TOP+56,WELL_LEFT+(WELL_WIDTH*8)-7,WELL_TOP+82, dst);
    ShowModal('   Game     Paused  ');
    SwapScreen;
    repeat 
        GetUserInput;
    until (key=KEY_P) or (key=KEY_ESC);
    SwapScreen;
    DrawImage(WELL_LEFT+6,WELL_TOP+56,WELL_LEFT+(WELL_WIDTH*8)-7,WELL_TOP+82, dst);
    gemdos_mfree(dst);
end;

procedure TitleScreen;
var f:byte;
begin
    ClearScreen(SCREEN_PHY);
    xbios_setpalette(@INTRO_PAL);
    UnApl(@INTRO, SCREEN_LOG);
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
    until (key=KEY_ENTER) or (key=KEY_SPACE) or quit;
    // UpdateStartingLevel(24);

    level := startingLevel;
    lines2levelup := level * LEVEL_UP_TRESHOLD;
end;
