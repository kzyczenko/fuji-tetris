from PIL import Image
import os

indir = 'images'
outdir = 'images'
fnum = 0

directory = os.fsencode(indir)
for file in os.listdir(directory):
    fname = os.fsdecode(file)
    if fname.endswith('.png'):
        print(f'*** procesing file {fname}')
        img = Image.open(f'{indir}/{fname}')
        w, h = img.size
        pixels = bytearray(img.getdata())
        palette = bytearray(img.getpalette())
        cnum = 0
        while len(pixels)>0:
            with open(f'{outdir}/slide{fnum}.c{cnum}', 'wb') as out_file:
                print(f'*** writing chunk {cnum} of {fname}')
                out_file.write(pixels[0:0x8000])
                pixels = pixels[0x8000:]
                cnum += 1
        with open(f'{outdir}/slide{fnum}.pal', 'wb') as out_file:
            print(f'*** saving palette for {fname}')
            out_file.write(palette)
        fnum += 1
        print(f'*** DONE\n')