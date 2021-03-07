

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


;******************************************************************************
;Date:			9 May 2013
;******************************************************************************

;- Test for the following standard:
;	- Logical operators (NOT, AND, OR, XOR) use only bit 0 of
;	  value.
;  If this is not the case, the assembler display a warning
;  message.
;****************************************************************

CONDITION_1	equ	1
CONDITION_2	equ	2
CONDITION_3	equ	3


	if 2
	nop
	endif

	if CONDITION_1
	nop
	endif

	if not CONDITION_1
	nop
	endif

	if not CONDITION_2
	nop
	endif

	if CONDITION_2 and CONDITION_3
	nop
	endif

	if CONDITION_2 or CONDITION_3
	nop
	endif

	if CONDITION_2 xor CONDITION_3
	nop
	endif


;******************************************************************************
;******************************************************************************







