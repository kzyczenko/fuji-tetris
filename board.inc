procedure NextTune; forward;

procedure ClrBoard;
begin
    FillChar(board, SizeOf(board), 0);
    FillChar(rowCount, SizeOf(rowCount), 0);
end;

procedure ClearBlock(xp,yp:TCellCoord; tiletype,rotation:byte);
var x,y: TCellCoord;
    i,cell: byte;
begin
    for i := 0 to 3 do begin
        cell := tile_cells[rotation,tiletype,i];
        x := xp + TCellCoord(cell and $0f);
        y := yp + TCellCoord(cell shr 4);
        if CellInsideWell(x,y) then
            ClearSquare(x,y);
    end;
end;

procedure DrawBlock(xp,yp:TCellCoord; tiletype,rotation:byte);
var x,y: TCellCoord;
    i,cell: byte;
begin
    for i := 0 to 3 do begin
        cell := tile_cells[rotation,tiletype,i];
        x := xp + TCellCoord(cell and $0f);
        y := yp + TCellCoord(cell shr 4);
        if CellInsideWell(x,y) then
            DrawSquare(x,y,tiletype+1);
    end;
end;

procedure CommitBlock(xp,yp:TCellCoord; tiletype,rotation:byte);
var x,y: TCellCoord;
    i,cell: byte;
begin
    for i := 0 to 3 do begin
        cell := tile_cells[rotation,tiletype,i];
        x := xp + TCellCoord(cell and $0f);
        y := yp + TCellCoord(cell shr 4);
        if CellInsideWell(x,y) then begin
            if board[x,y] = 0 then
                Inc(rowCount[y]);
            board[x,y] := tiletype+1;
        end;
    end;
end;

function CanMoveBlock(xp,yp:TCellCoord; tiletype,rotation:byte): boolean;
var x,y: TCellCoord;
    i,cell: byte;
    canMove: boolean;
begin
    canMove := true;
    i := 0;
    repeat
        cell := tile_cells[rotation,tiletype,i];
        x := xp + TCellCoord(cell and $0f);
        y := yp + TCellCoord(cell shr 4);
        if (x < 0) or (x >= WELL_WIDTH) then canMove := false;
        if (y >= WELL_HEIGHT) then canMove := false;
        if canMove and (y >= 0) and (board[x,y] <> 0) then canMove := false;
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

procedure DeleteRows(var rows: array of byte; linecount: byte);
var
    x: byte;
    src,dst,deletedIndex: smallint;
begin
    src := WELL_HEIGHT - 1;
    deletedIndex := linecount - 1;

    for dst := WELL_HEIGHT - 1 downto 0 do begin
        while (src >= 0) and (deletedIndex >= 0) and (src = rows[deletedIndex]) do begin
            Dec(src);
            Dec(deletedIndex);
        end;

        if src >= 0 then begin
            rowCount[dst] := rowCount[src];
            for x:=0 to WELL_WIDTH-1 do
                board[x,dst] := board[x,src];
            Dec(src);
        end else begin
            rowCount[dst] := 0;
            for x:=0 to WELL_WIDTH-1 do
                board[x,dst] := 0;
        end;
    end;
end;

procedure CheckRows(yr:TCellCoord);
var x,y,firstRow,lastRow: byte;
    rows: array [0..3] of byte;
    i,linecount: byte;
    bonus:cardinal;

begin
    linecount := 0;
    if yr < 0 then
        firstRow := 0
    else
        firstRow := yr;
    if yr + 4 > WELL_HEIGHT-1 then
        lastRow := WELL_HEIGHT-1
    else if yr + 4 < 0 then
        lastRow := 0
    else
        lastRow := yr + 4;

    for y:=firstRow to lastRow do
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
    if linecount > 0 then begin
        y:=rows[linecount-1];
        DeleteRows(rows, linecount);
        DrawRowsAfterDelete(y);
        SwapScreen;
        DrawRowsAfterDelete(y);
{$IFNDEF STE}
        ShakeScreen(1);
{$ENDIF}
    end;
{$IFDEF STE}
    if linecount > 0 then
        ShakeScreen(linecount);
{$ENDIF}

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
var targetY: TCellCoord;
begin
    ClearBlock(prevX1,prevY1,prevTile1,prevRotation1);
    ClearBlock(prevX2,prevY2,prevTile2,prevRotation2);
    SwapScreen;
    ClearBlock(prevX1,prevY1,prevTile1,prevRotation1);
    ClearBlock(prevX2,prevY2,prevTile2,prevRotation2);

    targetY := tileY;
    while CanMoveBlock(tileX, targetY + 1, currentTile, rotation) do
        Inc(targetY);
    tileY := targetY;

    DrawBlock(tileX, tileY, currentTile, rotation);
    SwapScreen;
    DrawBlock(tileX, tileY, currentTile, rotation);
    PlayDropSound;

    pieceDirty := false;
    fallcounter := 0;
end;
