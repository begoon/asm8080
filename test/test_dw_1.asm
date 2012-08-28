;******************************************************************************
;Filename:	test_dw_1.asm
;Description:	"DW" assembler directive test.
;Copyright(c):
;Author(s):	Claude Sylvain
;Created:	December 2010
;Last modified:	6 January 2012
;******************************************************************************


label_1
	nop

label_2
	nop


	dw	label_2
	dw	label_2 - label_1
	dw	label_2-label_1
	dw	label_2 - label_1, label_2 - label_1
	dw	label_2-label_1,label_2-label_1

	;Each characters pair must appear as a Little Endian word.
	;---------------------------------------------------------
	dw	'ABCD'
	dw	"ABCD"

	;Single character must appear as LSB on a Little Endian word.
	;------------------------------------------------------------
	dw	'A'
	dw	"A"
	dw	'ABC'
	dw	"ABC"

	dw	"Hello!"
	dw	"Hello!", "Hello!"
	dw	"Hello!", "Hello!", label_2 - label_1
	dw	"Hello!","Hello!",label_2-label_1
label_3
	dw	"Hello!","Hello!",label_2-label_1
label_4:
	dw	"Hello!","Hello!",label_2-label_1
label5
	dw	"Hello!","Hello!",label_2-label_1
	dw	$, 1






