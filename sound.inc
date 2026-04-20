{$IFDEF STE}
{$L dmasound.o}

function IsSteMachine: byte; external;
procedure DmaSoundInit; external;
procedure DmaSoundDone; external;
procedure DmaPlayRaw(sample: pointer; length: cardinal); external;

procedure InitSound;
const
{$i msx/sfx.inc}
begin
    soundReady := false;
    sfxDrop := @DROP_RAW_DATA[0];
    sfxRotate := @ROTATE_RAW_DATA[0];
    sfxShake[1] := @SHAKE1_RAW_DATA[0];
    sfxShake[2] := @SHAKE2_RAW_DATA[0];
    sfxShake[3] := @SHAKE3_RAW_DATA[0];
    sfxShake[4] := @SHAKE4_RAW_DATA[0];
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
    soundReady := true;
end;

procedure DoneSound;
begin
    if steMachine then
        DmaSoundDone;
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
