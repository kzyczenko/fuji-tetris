{$L unapl.o}

procedure UnApl(src, dst: pointer );external;  

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

function GetPaddedInt(i:cardinal;p:byte):string;
var tmp:string;
    o:byte;
begin
    FillChar(GetPaddedInt[1],p,'0');
    SetLength(GetPaddedInt,p);
    str(i,tmp);
    o:=p-Length(tmp);
    move(tmp[1],GetPaddedInt[o+1],Length(tmp));
end;

procedure LoadTopScore;
var handle, error: longint;
begin
    handle := gemdos_fopen('tetris.hsc', 0);
    if (handle > 0) then gemdos_fread(handle, 4, @topscore) else topscore:=1000;
    previous_topscore:=topscore;
    gemdos_fclose(handle);
end;

procedure SaveTopScore;
var handle: longint;
begin
    gemdos_fcreate('tetris.hsc',0);
    handle := gemdos_fopen('tetris.hsc', 1);
    gemdos_fwrite(handle, 4, @topscore);
    gemdos_fclose(handle);
end;

procedure Pause(count: word);
var i: word;
begin
    for i:=0 to count do xbios_vsync;
end;