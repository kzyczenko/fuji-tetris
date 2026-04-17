procedure NextTune; forward;

procedure ClrBoard;
begin
    FillChar(board, SizeOf(board), 0);
    FillChar(rowCount, SizeOf(rowCount), 0);
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

procedure ClearBlock(xp,yp,tiletype,rotation:byte);
var x,y,i,cell: byte;
begin
    for i := 0 to 3 do begin
        cell := tile_cells[rotation,tiletype,i];
        x := byte(xp + (cell and $0f));
        y := byte(yp + (cell shr 4));
        ClearSquare(x,y);
    end;
end;

procedure DrawBlock(xp,yp,tiletype,rotation:byte);
var x,y,i,cell: byte;
begin
    for i := 0 to 3 do begin
        cell := tile_cells[rotation,tiletype,i];
        x := byte(xp + (cell and $0f));
        y := byte(yp + (cell shr 4));
        DrawSquare(x,y,tiletype+1);
    end;
end;

procedure CommitBlock(xp,yp,tiletype,rotation:byte);
var x,y,i,cell: byte;
begin
    for i := 0 to 3 do begin
        cell := tile_cells[rotation,tiletype,i];
        x := byte(xp + (cell and $0f));
        y := byte(yp + (cell shr 4));
        if board[x,y] = 0 then
            Inc(rowCount[y]);
        board[x,y] := tiletype+1;
    end;
end;

function CanMoveBlock(xp,yp,tiletype,rotation:byte): boolean;
var x,y,i,cell: byte;
    canMove: boolean;
    localYp: byte;       // ← nowa lokalna zmienna
begin
    canMove := true;
    localYp := yp;       // ← kopia parametru

    if (localYp>WELL_HEIGHT) and (rotation > 0) then begin // I tile type fix
        Inc(localYp);    // ← modyfikuje TYLKO lokalną kopię
    end;

    i := 0;
    repeat
        cell := tile_cells[rotation,tiletype,i];
        x := byte(xp + (cell and $0f));
        y := byte(localYp + (cell shr 4));
        if (x > WELL_WIDTH-1) then canMove := false;
        if (y > WELL_HEIGHT-1) then canMove := false;
        if canMove and (board[x,y] <> 0) then canMove := false;
        inc(i);
    until (i=4) or (not canMove);

    CanMoveBlock := canMove;
end;

procedure DrawRowsAfterDelete(yr:byte);
var x,y,freelines: byte;
    freerow: boolean;
begin
    freelines := 0;
    for y:=yr downto 2 do begin
        freerow := true;
        for x:=0 to WELL_WIDTH-1 do begin
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

procedure DeleteRow(yr:byte);
var x,y,freelines: byte;
begin
    freelines := 0;
    for y:=yr downto 2 do begin
        rowCount[y] := rowCount[y-1];
        for x:=0 to WELL_WIDTH-1 do
            board[x,y] := board[x,y-1];
        if rowCount[y]=0 then begin
            Inc(freelines);
            if freelines>3 then exit;
        end;
    end;
end;

procedure CheckRows(yr:byte);
var x,y: byte;
    rows: array [0..3] of byte;
    i,linecount: byte;
    bonus:cardinal;

begin
    linecount := 0;
    for y:=yr to Min(yr+4, WELL_HEIGHT-1) do
        begin
            if rowCount[y] = WELL_WIDTH then
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
            bonus := bonus + (cardinal(50) * comboCounter * level);
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
        DeleteRow(y);
        DrawRowsAfterDelete(y);
        SwapScreen;
        DrawRowsAfterDelete(y);
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
var targetY: byte;
begin
    // Wyczyść stare pozycje
    ClearBlock(prevX1,prevY1,prevTile1,prevRotation1);
    ClearBlock(prevX2,prevY2,prevTile2,prevRotation2);
    SwapScreen;
    ClearBlock(prevX1,prevY1,prevTile1,prevRotation1);
    ClearBlock(prevX2,prevY2,prevTile2,prevRotation2);

    // Znajdź docelową pozycję
    targetY := tileY;
    while CanMoveBlock(tileX, targetY + 1, currentTile, rotation) do
        Inc(targetY);
    tileY := targetY;

    // Narysuj klocek na obu ekranach
    DrawBlock(tileX, tileY, currentTile, rotation);
    SwapScreen;
    DrawBlock(tileX, tileY, currentTile, rotation);

    pieceDirty := false;
    fallcounter := 0;
end;
