

	if 0

EQU_TEST1	equ	0
EQU_TEST2	equ	"Hello!"	;"EQU" do not support string.
EQU_TEST3	equ	'6'
EQU_TEST4	equ	2+2

;"EQU" work with value between 0 and 65535.
;
EQU_TEST5	equ	0FFFFFH

EQU_TEST6	equ	EQU_TEST4 + 1
EQU_TEST7	equ	EQU_TEST4+1
EQU_TEST8	equ	(EQU_TEST4 + 1)
EQU_TEST9	equ	(EQU_TEST4+1)
EQU_TEST10	equ	((EQU_TEST4 + 1))
EQU_TEST11	equ	((EQU_TEST4    +    1))
EQU_TEST12	equ	((EQU_TEST4	+	1))

EQU_TEST13	equ	((EQU_TEST4 + '0'))

EQU_TEST14	equ	0AAh + 03h
EQU_TEST14a	equ	0AAH + 03H
EQU_TEST15	equ	0AAh+03h
EQU_TEST16	equ	0AAh * 03h


EQU_TEST17	equ	(0AAh * 03h) + 1

EQU_TEST18	equ	1d
EQU_TEST19	equ	000000001d

	endif


	if 0

EQU_TEST20	equ	-1
EQU_TEST21	equ	-1-1
;EQU_TEST21	equ	(-1)-1
EQU_TEST22	equ	-1-1+1
;EQU_TEST22	equ	((-1)-1)+1
EQU_TEST23	equ	-1-1+3
;EQU_TEST23	equ	((-1)-1)+3

EQU_TEST24	equ	EQU_TEST4+EQU_TEST4
EQU_TEST25	equ	EQU_TEST4+EQU_TEST4+EQU_TEST4
EQU_TEST26	equ	EQU_TEST4 + EQU_TEST4 + EQU_TEST4
EQU_TEST27	equ	(EQU_TEST4 + EQU_TEST4) + EQU_TEST4
EQU_TEST28	EQU	(EQU_TEST4 + EQU_TEST4) + EQU_TEST4

EQU_TEST29	EQU	((EQU_TEST4 + EQU_TEST4) + EQU_TEST4) * 2
EQU_TEST30	EQU	(((EQU_TEST4 + EQU_TEST4) + EQU_TEST4) * 2) / 3

	endif



;my_symbol	set	10
my_symbol	equ	11
;my_symbol	set	12
my_symbol	equ	12
;my_symbol	set	11
;my_symbol	set	12
;my_symbol	set	13









