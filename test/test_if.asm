

;12 February 2011
;Test/Debug the following error:
;	*** Error 6 in "test_if.asm": 'if' nesting underflow!	
;************************************************************

	if 0

	if 0
	nop
	endif

	else

	nop

	endif


;Additionnal tests.
;******************

	if 1

	if 0
	nop
	endif

	else

	nop

	endif


	if 0

	if 1
	nop
	endif

	else

	nop

	endif


	if 1

	if 1
	nop
	endif

	else

	nop

	endif


	if 1

	if 0
	nop
	else
	nop
	endif

	else

	nop

	endif





