

;- Ref.: 8080-8085_assembly_language_programming__1977__intel__pdf_.pdf, 
;  page 185.


dsub_minu_start	ds	8
dsub_minu:		equ	$ - 1

dsub_sbtra_start	ds	8
dsub_sbtra		equ	$ - 1


dsub
	lxi	d, dsub_minu
	lxi	h, dsub_sbtra
	mvi	c, 8
	stc

dsub_00
	mvi	a, 99H
	aci	0
	sub	m
	xchg
	add	m
	daa
	mov	m, a
	xchg
	dcr	c
	jz	dsub_01

	inx	d
	inx	h
	jmp	dsub_00

dsub_01
	nop






