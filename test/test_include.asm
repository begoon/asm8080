

;	include	../src/test/empty.asm
;	include	../src/test/empty.asm		;This is a comment.
;	include	"../src/test/empty.asm"		;This is a comment.
;	include	../src/test/empty.asm
;	include	"../src/test/empty.asm"
	include	"empty.asm"
	include	empty2.asm
	include	empty3.asm			;This is a comment.


	org	0FFFEh


main
This_is_a_long_one


	mov	a, b


	end



	lxi	0
	mov	b, a




