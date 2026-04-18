{$L unapl.o}
{$L vbl.o}
{$L screen.o}

procedure UnApl(src, dst: pointer );external;  

procedure AlignScreenBuffer(rawScreen: pointer; var screen: pword); external;

function AddToVbl(callback: pointer): byte; external;

function RemoveFromVbl(callback: pointer): byte; external;

function TPred(rotation:byte): byte;
begin
    TPred := (rotation-1) and 3;
end;

function TSucc(rotation:byte): byte;
begin
    TSucc := (rotation+1) and 3;
end;

function Min(a,b:byte): byte;
begin
    if (a<b) then Min:=a else Min:=b;
end;

function CellInsideWell(x,y:TCellCoord): boolean;
begin
    CellInsideWell := (x >= 0) and (x < WELL_WIDTH) and (y >= 0) and (y < WELL_HEIGHT);
end;

function GetPaddedInt(i:cardinal; p:byte): string;
var
    tmp, padded: string;
    o, tlen: byte;
begin
    str(i, tmp);                           // konwersja liczby -> string
    tlen := Length(tmp);                   // <-- zapamiętaj raz, nie obliczaj dwa razy
    if tlen >= p then begin
        GetPaddedInt := Copy(tmp, tlen - p + 1, p);
    end else begin
        padded := '';
        SetLength(padded, p);
        FillChar(padded[1], p, '0');       // wypełnij zerami
        o := p - tlen;
        Move(tmp[1], padded[o+1], tlen);   // skopiuj cyfry na właściwe miejsce
        GetPaddedInt := padded;
    end;
end;

procedure LoadTopScore;
var handle: longint;
begin
    handle := gemdos_fopen('tetris.hsc', 0);
    if (handle > 0) then begin
        gemdos_fread(handle, 4, @topscore);
        gemdos_fclose(handle);
    end else begin
        topscore:=1000;
    end;
    previous_topscore:=topscore;
end;

procedure SaveTopScore;
var handle: longint;
begin
    handle := gemdos_fcreate('tetris.hsc',0);
    if (handle > 0) then begin
        gemdos_fwrite(handle, 4, @topscore);
        gemdos_fclose(handle);
    end;
end;

procedure Pause(count: word);
var i: word;
begin
    for i:=0 to count do xbios_vsync;
end;
