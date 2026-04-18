	export	AlignScreenBuffer

SCREEN_ALIGN_MASK	equ	$FFFFFF00
SCREEN_ALIGN_ADD	equ	255
SCREEN_SHAKE_BYTES	equ	1280

	TEXT

AlignScreenBuffer:
	move.l	a0,d0
	add.l	#SCREEN_ALIGN_ADD,d0
	and.l	#SCREEN_ALIGN_MASK,d0
	add.l	#SCREEN_SHAKE_BYTES,d0
	move.l	d0,(a1)
	rts

	END
