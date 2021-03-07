

;	include	../src/test/empty.asm
;	include	../src/test/empty.asm		;This is a comment.
;	include	"../src/test/empty.asm"		;This is a comment.
;	include	../src/test/empty.asm
;	include	"../src/test/empty.asm"
	include	"empty.asm"
	include	empty2.asm
	include	empty3.asm			;This is a comment.

	;- Include that file to test for ""END" directive found inside an
	;  include file!" Warning.
	;
;	include empty4.asm


	org	0FFFEh


main
This_is_a_long_one


	mov	a, b


	end



	lxi	0
	mov	b, a




