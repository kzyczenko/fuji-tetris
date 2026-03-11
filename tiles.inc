tile_O_0: array [0..3] of byte = 
                                    (1,1,
                                    1,1);

tile_I_0: array[0..15] of byte = 
                                    (0,0,0,0,
                                    1,1,1,1,
                                    0,0,0,0,
                                    0,0,0,0);
tile_I_1: array[0..15] of byte = 
                                    (0,0,1,0,
                                    0,0,1,0,
                                    0,0,1,0,
                                    0,0,1,0);
tile_I_2: array[0..15] of byte = 
                                    (0,0,0,0,
                                    0,0,0,0,
                                    1,1,1,1,
                                    0,0,0,0);
tile_I_3: array[0..15] of byte = 
                                    (0,1,0,0,
                                    0,1,0,0,
                                    0,1,0,0,
                                    0,1,0,0);

tile_T_0: array[0..8] of byte = 
                                    (0,1,0,
                                    1,1,1,
                                    0,0,0);

tile_T_1: array[0..8] of byte = 
                                    (0,1,0,
                                    0,1,1,
                                    0,1,0);

tile_T_2: array[0..8] of byte = 
                                    (0,0,0,
                                    1,1,1,
                                    0,1,0);

tile_T_3: array[0..8] of byte = 
                                    (0,1,0,
                                    1,1,0,
                                    0,1,0);

tile_S_0: array[0..8] of byte = 
                                    (0,1,1,
                                    1,1,0,
                                    0,0,0);

tile_S_1: array[0..8] of byte = 
                                    (0,1,0,
                                    0,1,1,
                                    0,0,1);

tile_S_2: array[0..8] of byte = 
                                    (0,0,0,
                                    0,1,1,
                                    1,1,0);

tile_S_3: array[0..8] of byte = 
                                    (1,0,0,
                                    1,1,0,
                                    0,1,0);

tile_Z_0: array[0..8] of byte = 
                                    (1,1,0,
                                    0,1,1,
                                    0,0,0);

tile_Z_1: array[0..8] of byte = 
                                    (0,0,1,
                                    0,1,1,
                                    0,1,0);

tile_Z_2: array[0..8] of byte = 
                                    (0,0,0,
                                    1,1,0,
                                    0,1,1);

tile_Z_3: array[0..8] of byte = 
                                    (0,1,0,
                                    1,1,0,
                                    1,0,0);

tile_L_0: array[0..8] of byte = 
                                    (0,0,1,
                                    1,1,1,
                                    0,0,0);

tile_L_1: array[0..8] of byte = 
                                    (0,1,0,
                                    0,1,0,
                                    0,1,1);

tile_L_2: array[0..8] of byte = 
                                    (0,0,0,
                                    1,1,1,
                                    1,0,0);

tile_L_3: array[0..8] of byte = 
                                    (1,1,0,
                                    0,1,0,
                                    0,1,0);

tile_J_0: array[0..8] of byte = 
                                    (1,0,0,
                                    1,1,1,
                                    0,0,0);

tile_J_1: array[0..8] of byte = 
                                    (0,1,0,
                                    0,1,0,
                                    1,1,0);

tile_J_2: array[0..8] of byte = 
                                    (0,0,0,
                                    1,1,1,
                                    0,0,1);

tile_J_3: array[0..8] of byte = 
                                    (0,1,1,
                                    0,1,0,
                                    0,1,0);

tiles_sizes: array [0..6] of byte =   (3,4,3,2,3,3,3);

tiles_0: array [0..6] of pointer =   (
                                        @tile_J_0, @tile_I_0, @tile_S_0, @tile_O_0, @tile_L_0, @tile_Z_0, @tile_T_0
                                        );
tiles_1: array [0..6] of pointer =   (
                                        @tile_J_1, @tile_I_1, @tile_S_1, @tile_O_0, @tile_L_1, @tile_Z_1, @tile_T_1
                                        );
tiles_2: array [0..6] of pointer =   (
                                        @tile_J_2, @tile_I_2, @tile_S_2, @tile_O_0, @tile_L_2, @tile_Z_2, @tile_T_2
                                        );
tiles_3: array [0..6] of pointer =   (
                                        @tile_J_3, @tile_I_3, @tile_S_3, @tile_O_0, @tile_L_3, @tile_Z_3, @tile_T_3
                                        );

nextXoff: array [0..6] of word = ( 13, 16, 13, 8, 13, 13, 13);  // było: 10,13,10,6,10,10,10
nextYoff: array [0..6] of word = ( 5, 10, 5, 5, 5, 5, 5);       // było: 4,8,4,4,4,4,4

tiles: array [0..3] of pointer =   (@tiles_0, @tiles_1, @tiles_2, @tiles_3);

tiles_all: array [0..3, 0..6] of pointer = (
    (@tile_J_0, @tile_I_0, @tile_S_0, @tile_O_0, @tile_L_0, @tile_Z_0, @tile_T_0),
    (@tile_J_1, @tile_I_1, @tile_S_1, @tile_O_0, @tile_L_1, @tile_Z_1, @tile_T_1),
    (@tile_J_2, @tile_I_2, @tile_S_2, @tile_O_0, @tile_L_2, @tile_Z_2, @tile_T_2),
    (@tile_J_3, @tile_I_3, @tile_S_3, @tile_O_0, @tile_L_3, @tile_Z_3, @tile_T_3)
);

function getTile(tiletype, rotation: byte): pointer; inline;
begin
    getTile := tiles_all[rotation, tiletype];
end;