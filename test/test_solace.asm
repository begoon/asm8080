;******************************************************************************
;Filename:	test_solace.asm
;Description:	Program to Test the Solace (SOL-20 emulator).
;Copyrigth(c):
;Author(s):	Claude Sylvain
;Created:	December 2011
;Last modified:	29 December 2011
;
;Notes:		- Solace (SOL-20 emulator) is available at the following
;		  place:
;			  http://www.sol20.org/solace.html
;******************************************************************************


;******************************************************************************
;				      EQU
;******************************************************************************

VDU_MEM_START	equ	0CC00h		;VDU Memory Start.
VDU_MEM_END	equ	0CFFFh		;VDU Memory End (last location).

;BIOS routines addresses.
;------------------------
BIOS_KBD_HR	equ	0C02Eh		;Keyboard Handler Routine.


;******************************************************************************
;				     MACROS
;******************************************************************************


;******************************************************************************
;Routine name:	GETCH
;Description:	Get key code from the keyboard, if any.
;Created:	29 December 2011
;Last modified:
;Parameters:	None.
;
;Returns:	Z:
;			0: Key code is available in A.
;			1: No key code available.
;
;Notes:		Use BIOS routine.
;******************************************************************************

GETCH	macro
	call	BIOS_KBD_HR	;Call BIOS Keyboard Handler Routine.
	endm


;******************************************************************************
;				      TEXT
;******************************************************************************

	org	0000h


;******************************************************************************
;Description:	The entry point.
;******************************************************************************

entry
	;Init. VDU.
	;----------
	mvi	a, 30h		;Init. VDU mem. with '0'.
	call	vdu_mem_init

	if 0
	;Wait for a key using "getchar" routine.
	;---------------------------------------
entry_00
	call	getchar		;Wait for a character...
;	cpi	'q'
	cpi	'Q'
	jnz	entry_00	;Exit on 'q'.
	endif

	if 0
	;Wait for a key using "getch" routine.
	;-------------------------------------
entry_00
	call	getch
	jz	entry_00
	cpi	'q'
	jnz	entry_00
	endif

	if 0
	;Wait for a key using "GETCH" macro.
	;-----------------------------------
entry_00
	GETCH
	jz	entry_00
	cpi	'q'
	jnz	entry_00
	endif


	;- Increment all VDU memory locations individually, until
	;  'q' key is pressed by the user.
	;*********************************************************

	lxi	hl, VDU_MEM_START

entry_00
	;Update VDU memory location.
	;---------------------------
	mov	a, m
	inr	a
	mov	m, a

	inx	hl			;Next VDU memory location.

	;Check for HL pointing end of VDU memory.
	;If so, make a roll over on HL.
	;----------------------------------------
	mov	a, h
	cpi	high (VDU_MEM_END + 1)
	jnz	entry_01
	mov	a, l
	cpi	low (VDU_MEM_END + 1)
	jnz	entry_01
	lxi	hl, VDU_MEM_START	;Roll  over!

entry_01
	call	delay256		;Delay!

	GETCH
	jz	entry_00
	cpi	'q'
	jnz	entry_00

	;Clean up VDU memory.
	;--------------------
	mvi	a, 20h		;Init. VDU mem. with space.
	call	vdu_mem_init

	ret			;Return control to system.


;******************************************************************************
;Routine name:	delay256
;Description:	Do a delay looping 256 times.
;Created:	29 December 2011
;Last modified:
;Parameters:	None.
;Returns:	None.
;Notes:
;******************************************************************************

delay256
;	mvi	a, 0
	xra	a
delay256_00
	dcr	a
	jnz	delay256_00
	ret


	if 0
;******************************************************************************
;Routine name:	getch
;Description:	Get key code from the keyboard, if any.
;Created:	29 December 2011
;Last modified:
;Parameters:	None.
;
;Returns:	Z:
;			0: Key code is available in A.
;			1: No key code available.
;
;Notes:		Use BIOS routine.
;******************************************************************************

getch
	call	BIOS_KBD_HR	;Call BIOS Keyboard Handler Routine.
	ret
	endif


;******************************************************************************
;Routine name:	getchar
;
;Description:	- Wait until a key is pressed on the keyboard.  Then
;		  return the key code.
;
;Created:	29 December 2011
;Last modified:
;Parameters:	None.
;
;Returns:	A:
;			Key code.
;
;Notes:		Use BIOS routine.
;******************************************************************************

getchar
	call	BIOS_KBD_HR	;Call BIOS Keyboard Handler Routine.
	jz	getchar
	ret


	if 0
;******************************************************************************
;Routine name:	kbhit
;Description:	Tell if a key was pressed.
;Created:	29 December 2011
;Last modified:
;Parameters:	None.
;
;Returns:	Z:
;			0: A key was pressed.
;			1: No key pressed.
;
;Notes:		Use BIOS routine.
;******************************************************************************

kbhit
	call	BIOS_KBD_HR	;Call BIOS Keyboard Handler Routine.
	ret
	endif


;******************************************************************************
;Routine name:	vdu_mem_init
;Description:	Initialize SOL-20 VDU memory.
;Created:	17 December 2011
;Last modified:	28 December 2011
;
;Parameters:	A:
;			Code to use to initialize VDU memory.
;
;Returns:	void
;******************************************************************************

vdu_mem_init:
	push	b
	push	h

	mov	b, a		;Save parameter.

	lxi	h, VDU_MEM_START

vdu_mem_init_00:
;	mvi	m, 41h		;Code to use to fill VDU Memory.
	mov	m, b		;Code to use to fill VDU Memory.
	inx	h		;Next VDU memory location.

	;Loop until (VDU memory + 1) is reached.
	;---------------------------------------
	mov	a,h
;	cpi	(VDU_MEM_END + 1) >> 8
	cpi	high (VDU_MEM_END + 1)
	jnz	vdu_mem_init_00
	mov	a,l
;	cpi	(VDU_MEM_END + 1) & 00FFh	;No more supported by asm8080 V1.01
;	cpi	(VDU_MEM_END + 1) and 00FFh
	cpi	low (VDU_MEM_END + 1)
	jnz	vdu_mem_init_00

	pop	h
	pop	b

	ret


;******************************************************************************
;******************************************************************************

	end




