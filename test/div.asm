

;- Ref.: 8080-8085_assembly_language_programming__1977__intel__pdf_.pdf, 
;  page 178.


div
	mov	a, d
	cma
	mov	d, a
	mov	a, e
	cma
	mov	e, a
	inx	d
	lxi	h, 0
	mvi	a, 17

div_00
	push	h
	dad	d
	jnc	div_01

	xthl

div_01
	pop	h
	push	psw
	mov	a, c
	ral
	mov	c, a
	mov	a, b
	ral
	mov	b, a
	mov	a, l
	ral
	mov	l, a
	mov	a, h
	ral
	mov	h, a
	pop	psw
	dcr	a
	jnz	div_00

	;Post-divide clean up.
	;Shift remainder right and return in DE.

	ora	a
	mov	a, h
	rar
	mov	d, a
	mov	a, l
	rar
	mov	e, a

	ret











