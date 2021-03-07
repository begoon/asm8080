

FALSE	equ	0
TRUE	equ	1


	db	high 1234h
	db	low 1234h

	db	15 mod 2

	;- '%' is a special character, and can not be
	;  implemented as a "C" like operator :-(
	;--------------------------------------------
	if 0
	db	15 % 2
	endif

	db	1 SHL 1
	db	1 SHL 2

	db	1 << 1
	db	1 << 2

	db	10000000b SHR 1
	db	10000000b SHR 2

	db	10000000b >> 1
	db	10000000b >> 2

	db	+1
	db	1+1

	db	-1
	db	2-1

	db	2 EQ 1
	db	1 EQ 1

	db	2 == 1
	db	1 == 1

	db	2 = 1
	db	1 = 1

	db	1 LT 1
	db	1 LT 2

	db	1 < 1
	db	1 < 2

	db	1 LE 0
	db	1 LE 1
	db	1 LE 2

	db	1 <= 0
	db	1 <= 1
	db	1 <= 2

	db	1 GT 1
	db	1 GT 0

	db	1 > 1
	db	1 > 0

	db	1 GE 2
	db	1 GE 0
	db	1 GE 1
	db	2 GE 1

	db	1 >= 2
	db	1 >= 0
	db	1 >= 1
	db	2 >= 1

	db	2 NE 1
	db	1 NE 1

	;- '!' is a special character, and can not be
	;  implemented as a "C" like operator :-(
	;--------------------------------------------
	if 0
	db	2 != 1
	db	1 != 1
	endif

	db	not 0
	dw	not 0
	db	not 0FFh
	dw	not 0FFh
	db	not 0FFFFh
	dw	not 0FFFFh

	;- '!' is a special character, and can not be
	;  implemented as a "C" like operator :-(
	;--------------------------------------------
	if 0
	db	!0
	db	!0FFh
	db	!0FFFFh
	endif

	;No more supported (2013-04-27).
	;-------------------------------
	if 0
	db	~0
	dw	~0
	db	~0FFh
	dw	~0FFh
	db	~0FFFFh
	dw	~0FFFFh
	endif

	db	0FFFFh and 5555h
	dw	0FFFFh and 5555h
	db	0 or 0AAAAh
	dw	0 or 0AAAAh
	db	0AAAAh xor 0AAAAh
	dw	0AAAAh xor 0AAAAh

	;- '&' is a special character, and can not be
	;  implemented as a "C" like operator :-(
	;--------------------------------------------
	if 0
	db	0FFFFh & 5555h
	db	0FFFFh && 5555h
	endif

	db	0 | 0AAAAh
	dw	0 | 0AAAAh
	db	0 || 0AAAAh
	dw	0 || 0AAAAh

	db	0AAAAh ^ 0AAAAh
	dw	0AAAAh ^ 0AAAAh

	db	TRUE and TRUE
	db	TRUE and FALSE
	db	TRUE or FALSE
	db	FALSE or FALSE
	db	FALSE OR FALSE
	db	FALSE OR TRUE
	db	(FALSE OR TRUE)


var_1	equ	3
var_2	equ	10


	db	(var_2 - var_1) shr 1
	db	-(var_2 - var_1)
	db	0-(var_2 - var_1)






