{$IFDEF STE}
{$L dmasound.o}

function IsSteMachine: byte; external;
procedure DmaSoundInit; external;
procedure DmaSoundDone; external;
procedure DmaPlayRaw(sample: pointer; length: cardinal); external;

function LoadRawSound(filename: pointer; size: cardinal): pointer;
var
    handle: longint;
    bytesRead: longint;
    buffer: pointer;
begin
    LoadRawSound := nil;
    buffer := gemdos_malloc(size);
    if buffer = nil then
        exit;

    handle := gemdos_fopen(filename, 0);
    if handle <= 0 then begin
        gemdos_mfree(buffer);
        exit;
    end;

    bytesRead := gemdos_fread(handle, size, buffer);
    if bytesRead = longint(size) then
        LoadRawSound := buffer
    else
        gemdos_mfree(buffer);
    gemdos_fclose(handle);
end;

procedure InitSound;
const
    DROP_RAW_SIZE = 1384;
    ROTATE_RAW_SIZE = 1798;
    SHAKE1_RAW_SIZE = 2074;
    SHAKE2_RAW_SIZE = 5968;
    SHAKE3_RAW_SIZE = 8553;
    SHAKE4_RAW_SIZE = 14958;
    DROP_RAW: array[0..12] of char = 'msx\drop.raw'#0;
    ROTATE_RAW: array[0..14] of char = 'msx\rotate.raw'#0;
    SHAKE1_RAW: array[0..15] of char = 'msx\1shake.raw'#0;
    SHAKE2_RAW: array[0..15] of char = 'msx\2shake.raw'#0;
    SHAKE3_RAW: array[0..15] of char = 'msx\3shake.raw'#0;
    SHAKE4_RAW: array[0..15] of char = 'msx\4shake.raw'#0;
begin
    soundReady := false;
    sfxDrop := nil;
    sfxRotate := nil;
    sfxShake[1] := nil;
    sfxShake[2] := nil;
    sfxShake[3] := nil;
    sfxShake[4] := nil;
    sfxDropLen := DROP_RAW_SIZE;
    sfxRotateLen := ROTATE_RAW_SIZE;
    sfxShakeLen[1] := SHAKE1_RAW_SIZE;
    sfxShakeLen[2] := SHAKE2_RAW_SIZE;
    sfxShakeLen[3] := SHAKE3_RAW_SIZE;
    sfxShakeLen[4] := SHAKE4_RAW_SIZE;
    steMachine := IsSteMachine <> 0;

    if not steMachine then
        exit;

    DmaSoundInit;
    sfxDrop := LoadRawSound(@DROP_RAW[0], DROP_RAW_SIZE);
    sfxRotate := LoadRawSound(@ROTATE_RAW[0], ROTATE_RAW_SIZE);
    sfxShake[1] := LoadRawSound(@SHAKE1_RAW[0], SHAKE1_RAW_SIZE);
    sfxShake[2] := LoadRawSound(@SHAKE2_RAW[0], SHAKE2_RAW_SIZE);
    sfxShake[3] := LoadRawSound(@SHAKE3_RAW[0], SHAKE3_RAW_SIZE);
    sfxShake[4] := LoadRawSound(@SHAKE4_RAW[0], SHAKE4_RAW_SIZE);
    soundReady := (sfxDrop <> nil) or (sfxRotate <> nil) or (sfxShake[1] <> nil) or (sfxShake[2] <> nil) or (sfxShake[3] <> nil) or (sfxShake[4] <> nil);
end;

procedure DoneSound;
var
    i: byte;
begin
    if steMachine then
        DmaSoundDone;
    if sfxDrop <> nil then
        gemdos_mfree(sfxDrop);
    if sfxRotate <> nil then
        gemdos_mfree(sfxRotate);
    for i := 1 to 4 do
        if sfxShake[i] <> nil then
            gemdos_mfree(sfxShake[i]);
    soundReady := false;
end;

procedure PlayDropSound;
begin
    if soundReady and (sfxDrop <> nil) then
        DmaPlayRaw(sfxDrop, sfxDropLen);
end;

procedure PlayRotateSound;
begin
    if soundReady and (sfxRotate <> nil) then
        DmaPlayRaw(sfxRotate, sfxRotateLen);
end;

procedure PlayShakeSound(level: byte);
begin
    if level < 1 then
        level := 1;
    if level > 4 then
        level := 4;
    if soundReady and (sfxShake[level] <> nil) then
        DmaPlayRaw(sfxShake[level], sfxShakeLen[level]);
end;
{$ELSE}
procedure InitSound;
begin
end;

procedure DoneSound;
begin
end;

procedure PlayDropSound;
begin
end;

procedure PlayRotateSound;
begin
end;

procedure PlayShakeSound(level: byte);
begin
    if level = 0 then
        exit;
end;
{$ENDIF}
