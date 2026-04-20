PPC = ppcross68k
AS = vasmm68k_mot
PYTHON = python3

TARGET = tetris.prg
STE_TARGET = tetriste.prg
MAIN = tetris.pas
SFX_INC = msx/sfx.inc
SFX_RAW = \
	msx/drop.raw \
	msx/rotate.raw \
	msx/1shake.raw \
	msx/2shake.raw \
	msx/3shake.raw \
	msx/4shake.raw

PFLAGS = -Tatari -O2 -Si -Xs
HATARI = hatari
HATARI_FLAGS = -w --monitor rgb --fastfdc true
HATARI_STE_FLAGS = $(HATARI_FLAGS) --machine ste

SOURCES = \
	$(MAIN) \
	const.inc \
	tiles.inc \
	palette.inc \
	random.inc \
	helpers.inc \
	sound.inc \
	fade.inc \
	gui.inc \
	board.inc \
	images/teamlogo.inc \
	images/logo.inc \
	images/intro.inc \
	msx/musici.inc \
	msx/music1.inc \
	msx/music2.inc \
	msx/music3.inc \
	msx/music4.inc \
	msx/music5.inc \
	msx/music6.inc \
	msx/musice.inc \
	graphics.o \
	sndhisr.o \
	unapl.o \
	screen.o \
	vbl.o

STE_SOURCES = \
	$(MAIN) \
	const.inc \
	tiles.inc \
	palette.inc \
	random.inc \
	helpers.inc \
	sound.inc \
	fade.inc \
	gui.inc \
	board.inc \
	images/teamlogo.inc \
	images/logo.inc \
	images/intro.inc \
	msx/hmusici.inc \
	msx/hmusic1.inc \
	msx/hmusic2.inc \
	msx/hmusic3.inc \
	msx/hmusice.inc \
	$(SFX_INC) \
	graphics.o \
	sndhisr.o \
	unapl.o \
	screen.o \
	vbl.o \
	dmasound.o

.PHONY: all clean run ste run-ste

all: $(TARGET)

$(TARGET): $(SOURCES) Makefile
	$(PPC) $(PFLAGS) $(MAIN)

graphics.o: graphics.s
	$(AS) -quiet -Faout -o $@ $<

unapl.o: unapl.s
	$(AS) -quiet -Faout -o $@ $<

sndhisr.o: sndhisr.s
	$(AS) -quiet -Faout -o $@ $<

screen.o: screen.s
	$(AS) -quiet -Faout -o $@ $<

vbl.o: vbl.s
	$(AS) -quiet -Faout -o $@ $<

dmasound.o: dmasound.s
	$(AS) -quiet -Faout -o $@ $<

$(SFX_INC): $(SFX_RAW) tools/raw2pas.py
	$(PYTHON) tools/raw2pas.py . $@

ste: $(STE_TARGET)

$(STE_TARGET): $(STE_SOURCES) Makefile
	$(PPC) $(PFLAGS) -dSTE -o$(STE_TARGET) $(MAIN)

clean:
	rm -f $(TARGET) $(STE_TARGET) tetris.o tetris.s *.o *.ppu *.map

run: $(TARGET)
	$(HATARI) $(HATARI_FLAGS) $(TARGET) &

run-ste: $(STE_TARGET)
	$(HATARI) $(HATARI_STE_FLAGS) $(STE_TARGET) &
