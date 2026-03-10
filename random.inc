procedure ShufflePool;
var
    t0:byte;
    r,s,d:byte;
begin
    poolsize:=0;
    for r:=0 to 6 do tilepool[r]:=r;
    repeat
        r := Random(5)+1; // 1..6
        d := (poolsize + r) mod 7; // offset to replace with
        s := tilepool[poolsize]; // temporary swap variable
        tilepool[poolsize]:=tilepool[d];
        tilepool[d]:=s;
        inc(poolsize);
        // writeln(r, ' ', d, ' ', s);
    until poolsize=7;
end;

function TossTile:byte;
begin
    if poolsize = 0 then ShufflePool;
    dec(poolsize);
    TossTile:=tilepool[poolsize];
    inc(stats[TossTile]);
end;