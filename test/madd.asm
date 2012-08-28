

;- Ref.: 8080-8085_assembly_language_programming__1977__intel__pdf_.pdf, 
;  page 180.


madd:
	lxi	b, madd_02
	lxi	h, madd_03
	xra	a

madd_00:
	ldax	b
	adc	m
	stax	b
	dcr	e
	jz	madd_01

	inx	b
	inx	h
	jmp	madd_00
madd_01:


madd_02:
	db	90H
	db	0BAH
	db	84H

madd_03:
	db	8AH
	db	0AFH
	db	32H




