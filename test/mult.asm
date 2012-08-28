

;- Ref.: 8080-8085_assembly_language_programming__1977__intel__pdf_.pdf, 
;  page 177.


mult:	mvi	b, 0
	mvi	e, 9

mult_00:
	mov	a, c
	rar
	mov	c, a
	dcr	e
	jz	mult_end
	mov	a, b
	jnc	mult_01
	add	d

mult_01:
	rar
	mov	b, a
	jmp	mult_00

mult_end:
	ret






