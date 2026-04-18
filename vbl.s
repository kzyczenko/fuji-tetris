	export	AddToVbl
	export	RemoveFromVbl

	TEXT

AddToVbl:
	movem.l	d1-d3/a1-a2,-(sp)
	move.w	sr,-(sp)
	ori.w	#$0700,sr

	moveq	#0,d0
	move.l	a0,d3
	beq.s	.addDone

	move.w	$454.w,d1		; nvbls
	beq.s	.addDone
	move.l	$456.w,a1		; _vblqueue
	move.l	a1,d2
	beq.s	.addDone

	subq.w	#1,d1
	suba.l	a2,a2
.addScan:
	move.l	(a1),d2
	beq.s	.addEmpty
	cmp.l	d2,d3
	beq.s	.addAlreadyInstalled
.addNext:
	addq.l	#4,a1
	dbra	d1,.addScan

	move.l	a2,d2
	beq.s	.addDone
	move.l	d3,(a2)
	moveq	#1,d0
	bra.s	.addDone

.addEmpty:
	move.l	a2,d2
	bne.s	.addNext
	move.l	a1,a2
	bra.s	.addNext

.addAlreadyInstalled:
	moveq	#1,d0

.addDone:
	move.w	(sp)+,sr
	movem.l	(sp)+,d1-d3/a1-a2
	rts

RemoveFromVbl:
	movem.l	d1-d3/a1,-(sp)
	move.w	sr,-(sp)
	ori.w	#$0700,sr

	moveq	#0,d0
	move.l	a0,d3
	beq.s	.removeDone

	move.w	$454.w,d1		; nvbls
	beq.s	.removeDone
	move.l	$456.w,a1		; _vblqueue
	move.l	a1,d2
	beq.s	.removeDone

	subq.w	#1,d1
.removeScan:
	move.l	(a1),d2
	cmp.l	d2,d3
	bne.s	.removeNext
	clr.l	(a1)
	moveq	#1,d0
.removeNext:
	addq.l	#4,a1
	dbra	d1,.removeScan

.removeDone:
	move.w	(sp)+,sr
	movem.l	(sp)+,d1-d3/a1
	rts

	END
