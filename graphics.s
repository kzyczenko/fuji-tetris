
	export	DrawPixel
	export	DrawBox
	export	BlitCachedSquare


DrawPixel:
	move.w	0(a1),d1				; Pixel X
	move.w	2(a1),d2				; Pixel Y
	move.l  a0,a1

	mulu.w  #160,d2
	add.l   d2,a1

	move.w	d1,d2								; X
	and.l	#$0000FFF0,d2						; To Nearest 16
	lsr.w	#1,d2								; 8 Byte Offset for bitplane
	add.l	d2,a1								; Get To X

	and.w	#15,d1								; X & 15
	add.w	d1,d1								; *2 As Offset In Table
	lea		gGraphic_4BP_Points,a0
	move.w	(a0,d1.w),d1						; Read Point
	move.w	d1,d2								; Copy Point
	not.w	d2									; Make Mask

	and.w	d2,(a1)								; Mask Plane 0
	and.w	d2,2(a1)							; Mask Plane 1
	and.w	d2,4(a1)							; Mask Plane 2
	and.w	d2,6(a1)							; Mask Plane 3

	lsr.w	#1,d0								; Check Bit 0
	bcc.s	.nbp0								; Not Set
	or.w	d1,6(a1)							; Draw To Plane0
.nbp0:
	lsr.w	#1,d0								; Check Bit 1
	bcc.s	.nbp1								; Not Set
	or.w	d1,4(a1)							; Draw To Plane 1
.nbp1:
	lsr.w	#1,d0								; Check Bit 2
	bcc.s	.nbp2								; Not Set
	or.w	d1,2(a1)							; Draw To Plane 2
.nbp2:
	lsr.w	#1,d0								; Check Bit 3
	bcc.s	.nbp3								; Not Set
	or.w	d1,0(a1)							; Draw To Plane 3
.nbp3:

	rts

DrawBox:
	movem.l	d3-d7/a2-a6,-(a7)					; save regs

	move.w	0(a1),d1				; Pixel X
	move.w	2(a1),d2				; Pixel Y
	move.w	4(a1),d3			; Width
	move.w	6(a1),d4			; Height
	move.l  a0,a2
	move.l	#160,a6	; line width

	subq.w	#1,d4								; -1 for dbra

	mulu.w  #160,d2								; Y * 2
	add.l   d2,a2						; Get To Y

	move.w	d1,d2								; X
	and.l	#$0000FFF0,d2						; To Nearest 16
	lsr.w	#1,d2								; 8 Byte Offset for bitplane
	add.l	d2,a2								; Get To X

	and.l	#15,d0								; Colour
	lsl.w	#3,d0								; Colour * 8
	lea		Graphic_4BP_ColourChunks,a0			; colour chunks
	add.l	d0,a0

	moveq	#-1,d5

	moveq	#15,d2								; mask
	and.w	d1,d2								; X & 15
	beq.s	.noLeft								; 16 pixel aligned, no left strip

	move.w	d2,d0								; X0 &15

	add.w	d2,d2								; X offset *2
	add.w	d2,d2								; X offset *4
	lea		gGraphic_4BP_DoubleLeftMasks,a1
	move.l	(a1,d2.w),d5						; double mask

	add.w	d3,d0								; +Width
	cmp.w	#16,d0								; into next plane?
	bge		.noDClip

	add.w	d0,d0								; X offset *2
	add.w	d0,d0								; X offset *4
	lea		gGraphic_4BP_DoubleRightMasks-4,a1
	and.l	(a1,d0.w),d5						; double mask

.noDClip:

	move.l	(a0),d0								; Plane 0.1
	move.l	4(a0),d2							; Plane 2.3
	and.l	d5,d0								; Mask Planes 0.1
	and.l	d5,d2								; Mask Planes 1.2
	not.l	d5

	move.w	d4,d6								; Height
	move.l	a2,a1								; pVRAM
	move.l	a6,a3								; Line Width
	subq.l	#4,a3								; -4
.leftLoop:
	and.l	d5,(a1)								; Mask Dst Planes 0.1
	or.l	d0,(a1)+							; Draw Planes 0.1
	and.l	d5,(a1)								; Mask Planes 1.2
	or.l	d2,(a1)								; Draw Planes 1.2
	add.l	a3,a1								; Next Scanline
	dbra	d6,.leftLoop						; Loop For Box Height

	addq.l	#8,a2								; Get to Next bitplanes

	moveq	#15,d0								; 15
	and.w	d1,d0								; X & 15
	moveq	#16,d2								; 16
	sub.w	d0,d2								; 16-(X&15) = num pixels drawn
	add.w	d2,d1								; New X Pos
	sub.w	d2,d3								; Dec Width
	ble		.fin								; All Drawn

.noLeft:

	move.w	d3,d6								; width
	lsr.w	#4,d6								; /16
	beq		.noMid

	move.w	d6,d0								; num chunks
	lsl.w	#3,d0								; *chunk size
	move.l	a6,a5
	sub.w	d0,a5								; adjust line offset

	move.l	a2,a1								; pVRAM
	lea		(a2,d0.w),a2						; scren adr for right strip
	subq.w	#1,d6								; -1 for dbra

	move.l	(a0),d0								; Plane 0.1
	move.l	4(a0),d2							; Plane 2.3

	move.w	d4,d7								; height
.midLoopY:
	move.w	d6,d5								; Init X Loop
.midLoopX:
	move.l	d0,(a1)+							; Draw Planes 0.1
	move.l	d2,(a1)+							; Draw Planes 2.3
	dbra	d5,.midLoopX						; Loop For X
	add.l	a5,a1								; Next Line
	dbra	d7,.midLoopY						; Loop For Y


.noMid:

	and.w	#15,d3								; dec width
	beq		.fin

	add.w	d3,d3								; X offset *2
	add.w	d3,d3								; X offset *4
	lea		gGraphic_4BP_DoubleRightMasks-4,a1
	move.l	(a1,d3.w),d5						; double mask

	move.l	(a0),d0								; Plane 0.1
	move.l	4(a0),d2							; Plane 2.3
	and.l	d5,d0								; Mask Planes 0.1
	and.l	d5,d2								; Mask Planes 2.3
	not.l	d5

	subq.l	#4,a6								; LineSize -4
.rightLoop:
	and.l	d5,(a2)								; Mask Planes 0.1
	or.l	d0,(a2)+							; Draw Planes 0.1
	and.l	d5,(a2)								; Mask Planes 2.3
	or.l	d2,(a2)								; Draw Planes 2.3
	add.l	a6,a2								; Next Scanline
	dbra	d4,.rightLoop						; Loop For Box Height


.fin:

	movem.l	(a7)+,d3-d7/a2-a6					; restore registers
	rts

BlitCachedSquare:
	movem.l	d3-d4/a2-a3,-(a7)

	move.l	0(a1),a2							; cached tile data
	move.l	4(a1),a3							; 2 word masks
	moveq	#0,d4
	move.w	8(a1),d4							; screen byte offset
	add.l	d4,a0								; destination

	moveq	#9,d3								; 10 lines
.squareLine:
	move.w	(a3),d2								; group 0 mask
	beq.s	.skipGroup0
	move.w	d2,d0
	not.w	d0

	move.w	(a0),d1
	and.w	d0,d1
	move.w	(a2)+,d4
	and.w	d2,d4
	or.w	d4,d1
	move.w	d1,(a0)

	move.w	2(a0),d1
	and.w	d0,d1
	move.w	(a2)+,d4
	and.w	d2,d4
	or.w	d4,d1
	move.w	d1,2(a0)

	move.w	4(a0),d1
	and.w	d0,d1
	move.w	(a2)+,d4
	and.w	d2,d4
	or.w	d4,d1
	move.w	d1,4(a0)

	move.w	6(a0),d1
	and.w	d0,d1
	move.w	(a2)+,d4
	and.w	d2,d4
	or.w	d4,d1
	move.w	d1,6(a0)
	bra.s	.group0Done
.skipGroup0:
	addq.l	#8,a2
.group0Done:

	move.w	2(a3),d2							; group 1 mask
	beq.s	.skipGroup1
	move.w	d2,d0
	not.w	d0

	move.w	8(a0),d1
	and.w	d0,d1
	move.w	(a2)+,d4
	and.w	d2,d4
	or.w	d4,d1
	move.w	d1,8(a0)

	move.w	10(a0),d1
	and.w	d0,d1
	move.w	(a2)+,d4
	and.w	d2,d4
	or.w	d4,d1
	move.w	d1,10(a0)

	move.w	12(a0),d1
	and.w	d0,d1
	move.w	(a2)+,d4
	and.w	d2,d4
	or.w	d4,d1
	move.w	d1,12(a0)

	move.w	14(a0),d1
	and.w	d0,d1
	move.w	(a2)+,d4
	and.w	d2,d4
	or.w	d4,d1
	move.w	d1,14(a0)
	bra.s	.group1Done
.skipGroup1:
	addq.l	#8,a2
.group1Done:

	lea		160(a0),a0
	dbra	d3,.squareLine

	movem.l	(a7)+,d3-d4/a2-a3
	rts

**************************************************************************************
	DATA
**************************************************************************************

gGraphic_4BP_DoubleLeftMasks:
	dc.w	$FFFF,$FFFF
	dc.w	$7FFF,$7FFF
	dc.w	$3FFF,$3FFF
	dc.w	$1FFF,$1FFF
	dc.w	$0FFF,$0FFF
	dc.w	$07FF,$07FF
	dc.w	$03FF,$03FF
	dc.w	$01FF,$01FF
	dc.w	$00FF,$00FF
	dc.w	$007F,$007F
	dc.w	$003F,$003F
	dc.w	$001F,$001F
	dc.w	$000F,$000F
	dc.w	$0007,$0007
	dc.w	$0003,$0003
	dc.w	$0001,$0001

gGraphic_4BP_DoubleRightMasks:
	dc.w	$8000,$8000
	dc.w	$C000,$C000
	dc.w	$E000,$E000
	dc.w	$F000,$F000
	dc.w	$F800,$F800
	dc.w	$FC00,$FC00
	dc.w	$FE00,$FE00
	dc.w	$FF00,$FF00
	dc.w	$FF80,$FF80
	dc.w	$FFC0,$FFC0
	dc.w	$FFE0,$FFE0
	dc.w	$FFF0,$FFF0
	dc.w	$FFF8,$FFF8
	dc.w	$FFFC,$FFFC
	dc.w	$FFFE,$FFFE
	dc.w	$FFFF,$FFFF

Graphic_4BP_ColourChunks:
	dc.w	$0000,$0000,$0000,$0000
	dc.w	$FFFF,$0000,$0000,$0000
	dc.w	$0000,$FFFF,$0000,$0000
	dc.w	$FFFF,$FFFF,$0000,$0000
	dc.w	$0000,$0000,$FFFF,$0000
	dc.w	$FFFF,$0000,$FFFF,$0000
	dc.w	$0000,$FFFF,$FFFF,$0000
	dc.w	$FFFF,$FFFF,$FFFF,$0000
	dc.w	$0000,$0000,$0000,$FFFF
	dc.w	$FFFF,$0000,$0000,$FFFF
	dc.w	$0000,$FFFF,$0000,$FFFF
	dc.w	$FFFF,$FFFF,$0000,$FFFF
	dc.w	$0000,$0000,$FFFF,$FFFF
	dc.w	$FFFF,$0000,$FFFF,$FFFF
	dc.w	$0000,$FFFF,$FFFF,$FFFF
	dc.w	$FFFF,$FFFF,$FFFF,$FFFF

gGraphic_4BP_Points:
	dc.w	$8000
	dc.w	$4000
	dc.w	$2000
	dc.w	$1000
	dc.w	$0800
	dc.w	$0400
	dc.w	$0200
	dc.w	$0100
	dc.w	$0080
	dc.w	$0040
	dc.w	$0020
	dc.w	$0010
	dc.w	$0008
	dc.w	$0004
	dc.w	$0002
	dc.w	$0001
