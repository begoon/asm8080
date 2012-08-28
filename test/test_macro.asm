;******************************************************************************
;Filename:	test_macro.asm
;Description:	Assembly module to test asm8080 "MACRO" assembly directive.
;Copyright(c):
;Author(s):	Claude Sylvain
;Created:	28 December 2011
;Last modified:	29 December 2011
;******************************************************************************


;******************************************************************************
;******************************************************************************

;Empty macro.
;------------
M_EMPTY	macro
	endm


;NOP3	macro	x
NOP3	macro

	nop
	nop
	nop

	endm


CLRA	macro
	if 0
	mvi	a, 0
	endif
	xra	a
	endm


TSTA	macro
	ora	a
	endm


;******************************************************************************
;Macro name:	DELAY_1
;Description:
;Created:	28 December 2011
;Last modified:	29 December 2011
;******************************************************************************

DELAY_1	macro

;	xra	a
	CLRA

delay_1_00
	dcr	a
	jnz	delay_1_00	;Loop if delay not ended.

	endm


;******************************************************************************
;******************************************************************************

	mvi	a, 0


	NOP3
	CLRA
	TSTA
	DELAY_1
	M_EMPTY


;******************************************************************************
;******************************************************************************

	db	0, 1









