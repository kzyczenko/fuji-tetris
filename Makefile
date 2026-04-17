PPC = ppcross68k
AS = vasmm68k_mot

TARGET = tetris.prg
HOSPES_TARGET = tetris_hospes.prg
MAIN = tetris.pas

PFLAGS = -Tatari -O2 -Si -Xs
HATARI = hatari
HATARI_FLAGS = -w --monitor rgb --fastfdc true

SOURCES = \
	$(MAIN) \
	const.inc \
	tiles.inc \
	palette.inc \
	random.inc \
	helpers.inc \
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
	unapl.o

HOSPES_SOURCES = \
	$(MAIN) \
	const.inc \
	tiles.inc \
	palette.inc \
	random.inc \
	helpers.inc \
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
	graphics.o \
	sndhisr.o \
	unapl.o

.PHONY: all clean run hospes

all: $(TARGET)

$(TARGET): $(SOURCES) Makefile
	$(PPC) $(PFLAGS) $(MAIN)

graphics.o: graphics.s
	$(AS) -quiet -Faout -o $@ $<

unapl.o: unapl.s
	$(AS) -quiet -Faout -o $@ $<

sndhisr.o: sndhisr.s
	$(AS) -quiet -Faout -o $@ $<

hospes: $(HOSPES_TARGET)

$(HOSPES_TARGET): $(HOSPES_SOURCES) Makefile
	$(PPC) $(PFLAGS) -dHOSPES -o$(HOSPES_TARGET) $(MAIN)

clean:
	rm -f $(TARGET) $(HOSPES_TARGET) tetris.o tetris.s a.out *.ppu *.map

run: $(TARGET)
	$(HATARI) $(HATARI_FLAGS) $(TARGET) &
