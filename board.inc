procedure NextTune; forward;

procedure ClrBoard;
var x,y: byte;
begin
    x:=0;
    repeat
        y:=0;
        repeat
            board[x,y] := 0;
            inc(y);
        until y=WELL_HEIGHT;
        inc(x);
    until x=WELL_WIDTH;
end;

// procedure DrawBoard;
// var x,y: byte;
// begin
//     x:=0;
//     repeat
//         y:=0;
//         repeat
//             SetLength(s,0);
//             s := GetPaddedInt(board[x,y],2);
//             NeoDrawRect(x*16,y*8,x*16+14,y*8+8,2);
//             NeoDrawString(x*16,y*8,8,s);
//             inc(y);
//         until y=WELL_HEIGHT;
//         inc(x);
//     until x=WELL_WIDTH;
// end;

procedure DrawBlock(xp,yp,tiletype,rotation,option:byte);
var x,y,i: byte;
    tile: word;
    tilebody: pword;
begin
    i := 0;
    tilebody := getTile(tiletype,rotation);
    y:=0;
    repeat
        x:=0;
        repeat
            tile := (tilebody+i)^;
            if tile = 1 then 
                case option of 
                    0: ClearSquare(byte(x+xp),byte(y+yp));
                    1: DrawSquare(byte(x+xp),byte(y+yp),tiletype+1);
                    2: board[byte(x+xp),byte(y+yp)] := tiletype+1;
                end;
            inc(i);
            inc(x);
        until x=tiles_sizes[tiletype];
        Inc(y);
    until y=tiles_sizes[tiletype];      
end;

function CanMoveBlock(xp,yp,tiletype,rotation:byte): boolean;
var x,y,i: byte;
    tile: word;
    tilebody: pword;
begin
    CanMoveBlock := true;
    i := 0;
    tilebody := getTile(tiletype,rotation);

    if (tileY>WELL_HEIGHT) and (rotation > 0) then begin // I tile type fix
        Inc(tileY);
        Inc(yp);
    end;
    y:=0;
    repeat
        x:=0;
        repeat
            tile := (tilebody+i)^;
            if ( tile=1 )then
                begin
                    if (byte(x+xp) > WELL_WIDTH-1 )then CanMoveBlock := false;
                    if (byte(y+yp) > WELL_HEIGHT-1 )then CanMoveBlock := false;
                    if CanMoveBlock and ( board[byte(x+xp),byte(y+yp)]<>0 ) then CanMoveBlock := false;
                end;  
            inc(i);
            inc(x);
        until x=tiles_sizes[tiletype];
        Inc(y);
    until y=tiles_sizes[tiletype];      
end;

procedure DeleteRow(yr:byte);
var x,y,freelines: byte;
    freerow: boolean;
begin
    // for x:=0 to WELL_WIDTH-1 do
        // begin
        //     ClearSquare(x,y);
        // end;

    freelines := 0;
    for y:=yr downto 2 do 
        begin
            freerow := true;
            for x:=0 to WELL_WIDTH-1 do
                begin
                    board[x,y] := board[x,y-1];
                    if ( board[x,y]=0 ) then begin
                        ClearSquare(x,y);
                    end else begin  
                        DrawSquare(x,y,board[x,y]);
                        freerow := false;
                    end;
                end;
            if freerow then begin
                Inc(freelines);
                if freelines>3 then exit;
            end;
        end;
end;

procedure CheckRows(yr:byte);
var x,y,sfx: byte;
    del: boolean;
    rows: array [0..3] of byte;
    i,linecount: byte;
    bonus:cardinal;
    board_bak: array[0..WELL_WIDTH-1,0..WELL_HEIGHT-1] of byte;

begin
    linecount := 0;
    for y:=yr to Min(yr+4, WELL_HEIGHT-1) do
        begin
            del := true;

            for x:=0 to WELL_WIDTH-1 do
               if ( board[x,y]=0 )then del := false;

            if del then
                begin
                    rows[linecount]:=y;
                    inc(linecount);
                end;
        end;

    // at least one line is filled
    if linecount>0 then begin
        
        // regular score
        bonus := scoretable[linecount-1] * level;
        crackCounter := 0;
        
        // combo score
        if comboCounter>0 then begin
            bonus := bonus + (50 * comboCounter * level);
        end;
        Inc(comboCounter);
        
        score := score + bonus;
        if score > topscore then topscore := score;
        DrawCounters;
        SwapScreen;
        DrawCounters;

        // increase line counters
        levelLines := levelLines + linecount;
        totalLines := totalLines + linecount;
    end else begin
        comboCounter := 0;
    end;

    // animate disapearing tiles
    for x:=(WELL_WIDTH div 2) to WELL_WIDTH-1 do begin
        for i:=1 to linecount do begin
            y:=rows[i-1];
            ClearSquarePhy(x,y);
            ClearSquarePhy(WELL_WIDTH-1-x,y);
            xbios_vsync;
        end;
    end;

    // fall down rows
    for i:=1 to linecount do begin
        y:=rows[i-1];
        board_bak:=board;
        DeleteRow(y);
        board:=board_bak;
        SwapScreen;
        DeleteRow(y);
        ShakeScreen;
    end;

    // check if level goal reached
    if ( levelLines>=lines2levelup )then
        begin
            levelLines := levelLines - lines2levelup;
            lines2levelup := LEVEL_UP_TRESHOLD;
            level := level + 1;
            FlashBackground;
            inc(next_tune_counter);
            if next_tune_counter > 2 then begin
                NextTune;
                next_tune_counter := 0;
            end;
            DrawCounters;
            SwapScreen;
            DrawCounters;
        end;
end;

procedure FastFall;
begin
    DrawBlock(prevX1,prevY1,prevTile1,prevRotation1,0);
    DrawBlock(prevX2,prevY2,prevTile2,prevRotation2,0);
    SwapScreen;
    DrawBlock(prevX1,prevY1,prevTile1,prevRotation1,0);
    DrawBlock(prevX2,prevY2,prevTile2,prevRotation2,0);
    while CanMoveBlock(tileX,tileY+1,currentTile,rotation) do begin
        DrawBlock(tileX,tileY,currentTile,rotation,0);
        tileY := tileY + 1;
        DrawBlock(tileX,tileY,currentTile,rotation,1);
        tileY := tileY - 1;
        SwapScreen;
        DrawBlock(tileX,tileY,currentTile,rotation,0);
        tileY := tileY + 1;
        DrawBlock(tileX,tileY,currentTile,rotation,1);
    end;
    fallcounter := 0;
end;
