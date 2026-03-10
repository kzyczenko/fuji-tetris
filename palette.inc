
const 
palette: array [0..15] of word = (
    $992, $111, $000, $888, $333, $aaa, $ddd, $bbb,
    $ddb, $0f0, $0ff, $00c, $f0f, $f00, $fc0, $ff0
);

procedure LoadPalette;
var c:byte;
begin
    xbios_setpalette(@palette);
end;

