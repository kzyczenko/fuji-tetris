	export	IsSteMachine
	export	DmaSoundInit
	export	DmaSoundDone
	export	DmaPlayRaw

	TEXT

GetCookieValue:
	moveq	#0,d0
	move.l	$5a0.w,d1
	beq.s	.cookieDone
	move.l	d1,a0
.cookieLoop:
	move.l	(a0)+,d1
	beq.s	.cookieDone
	move.l	(a0)+,d0
	cmp.l	d2,d1
	bne.s	.cookieLoop
	bra.s	.cookieFound
.cookieDone:
	moveq	#0,d0
.cookieFound:
	rts

IsSteMachine:
	movem.l	d1-d2/a0,-(sp)
	move.l	#$5f4d4348,d2			; '_MCH'
	bsr.s	GetCookieValue
	cmp.l	#$00010000,d0			; STe
	beq.s	.isSte
	cmp.l	#$00010008,d0			; ST Book
	beq.s	.isSte
	cmp.l	#$00010010,d0			; Mega STe
	beq.s	.isSte

	move.l	#$5f56444f,d2			; '_VDO'
	bsr.s	GetCookieValue
	swap	d0
	cmp.w	#1,d0
	beq.s	.isSte

	moveq	#0,d0
	bra.s	.isSteDone
.isSte:
	moveq	#1,d0
.isSteDone:
	movem.l	(sp)+,d1-d2/a0
	rts

DmaSoundInit:
	move.w	sr,-(sp)
	ori.w	#$0700,sr
	move.b	$ffff8901.w,sDmaControl
	move.b	$ffff8921.w,sDmaMode
	clr.b	$ffff8901.w
	move.w	(sp)+,sr
	rts

DmaSoundDone:
	move.w	sr,-(sp)
	ori.w	#$0700,sr
	clr.b	$ffff8901.w
	move.b	sDmaMode,$ffff8921.w
	move.b	sDmaControl,$ffff8901.w
	move.w	(sp)+,sr
	rts

DmaPlayRaw:
	movem.l	d1/a0,-(sp)
	move.l	a0,d1
	beq.s	.playDone
	tst.l	d0
	beq.s	.playDone

	move.w	sr,-(sp)
	ori.w	#$0700,sr

	clr.b	$ffff8901.w

	move.b	d1,$ffff8907.w
	lsr.l	#8,d1
	move.b	d1,$ffff8905.w
	lsr.l	#8,d1
	move.b	d1,$ffff8903.w

	move.l	a0,d1
	add.l	d0,d1
	move.b	d1,$ffff8913.w
	lsr.l	#8,d1
	move.b	d1,$ffff8911.w
	lsr.l	#8,d1
	move.b	d1,$ffff890f.w

	move.b	#$81,$ffff8921.w		; 12517 Hz, 8-bit, mono
	move.b	#1,$ffff8901.w			; DMA replay on, no loop

	move.w	(sp)+,sr
.playDone:
	movem.l	(sp)+,d1/a0
	rts

sDmaControl:
	dc.b	0
sDmaMode:
	dc.b	0
	even

	END
