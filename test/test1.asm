

;
;Copyright (c) <2007-2011> <jay.cotton@oracle.com>
;
;Permission is hereby granted, free of charge, to any person
;obtaining a copy of this software and associated documentation
;files (the "Software"), to deal in the Software without
;restriction, including without limitation the rights to use,
;copy, modify, merge, publish, distribute, sublicense, and/or sell
;copies of the Software, and to permit persons to whom the
;Software is furnished to do so, subject to the following
;conditions:
;
;The above copyright notice and this permission notice shall be
;included in all copies or substantial portions of the Software.
;
;THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
;EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
;OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
;NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
;HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
;WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
;FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
;OTHER DEALINGS IN THE SOFTWARE.
;


; Test file used to checkout the assembler 


	;- Not standard, but supported if asm8080 is build with
	;  support for language extension.
	;
	DB	"Test Test Test"

	DB	'Test Test Test'


FRONT	EQU	$
BACK	EQU	FRONT
XYZ	EQU	7


TEST	MOV A,B  
TES1	MOV A,C  

	;This should load HL with a value of 000Dh.
	;
	LXI	HL,((XYZ + 20)+TEST)/4

	MOV A,D  
	MOV A,E  
	MOV A,H  
	MOV A,L  
	MOV A,M  
	MOV B,A  
	MOV B,C  
	MOV B,D  
	MOV B,E  
	MOV B,H  
	MOV B,L  
	MOV B,M  
	MOV C,A  
	MOV C,B  
	MOV C,D  
	MOV C,E  
	MOV C,H  
	MOV C,L  
	MOV C,M  
	MOV D,A  
	MOV D,B  
	MOV D,C  
	MOV D,E  
	MOV D,H  
	MOV D,L  
	MOV D,M  
	MOV E,A  
	MOV E,B  
	MOV E,C  
	MOV E,D  
	MOV E,H  
	MOV E,L  
	MOV E,M  
	MOV H,A  
	MOV H,B  
	MOV H,C  
	MOV H,D  
	MOV H,E  
	MOV H,L  
	MOV H,M  
	MOV L,A  
	MOV L,B  
	MOV L,C  
	MOV L,D  
	MOV L,E  
	MOV L,H  
	MOV L,M  
	MOV M,A  
	MOV M,B  
	MOV M,C  
	MOV M,D  
	MOV M,E  
	MOV M,H  
	MOV M,L  
TEST2	MVI A,25
	MVI B,2
	MVI C,2
TEST4	MVI D,2
	MVI D,'T'
	MVI D,XYZ
	MVI E,2
	MVI H,2
	MVI L,2
	MVI M,2
;TEST3	LXI BC,0x2233 
TEST3	LXI BC,2233h
;	LXI DE,0x2233 
	LXI DE,2233h
;	LXI HL,0x2233 
	LXI HL,2233h
;	LXI SP,0x2233 
	LXI SP,2233h
	LXI SP,TEST4
;	LXI SP,"DF"
;	LDA 0x1234
	LDA 1234h
	LDA TEST3 
;	LDA "xyay"
	LDA 'l' 
	LDA $
	LDA $+8
	LDA $-8
	LDA $+'T'
	LDA $+TEST3	
	LDA $-TEST3	
;	STA 0x1234
	STA 1234h
	STA TEST3 
;	STA "xyay"
	STA 'l' 
	STA $
	STA $+8
	STA $-8
	STA $+'T'
	STA $+TEST3	
	STA $-TEST3	
;	LHLD 0x1234
	LHLD 1234h
	LHLD TEST3 
;	LHLD "xyay"
	LHLD 'l' 
	LHLD $
	LHLD $+8
	LHLD $-8
	LHLD $+'T'
	LHLD $+TEST3	
	LHLD $-TEST3	
;	SHLD 0x1234
	SHLD 1234h
	SHLD TEST3 
;	SHLD "xyay"
	SHLD 'l' 
	SHLD $
	SHLD $+8
	SHLD $-8
	SHLD $+'T'
	SHLD $+TEST3	
	SHLD $-TEST3	
	LDAX BC 
	LDAX DE 
	STAX BC 
	STAX DE 
	XCHG 
	ADD A  
	ADD B  
	ADD C  
	ADD D  
	ADD E  
	ADD H  
	ADD L  
	ADD M  
	ADI low $
	ADI 4 
;	ADI 0x45   
	ADI 45h
	ADI 'T'   
	ADI XYZ
	ADC A   
	ADC B   
	ADC C   
	ADC D   
	ADC E   
	ADC H   
	ADC L   
	ADC M   
;	ACI    $
	ACI 4 
;	ACI 0x45   
	ACI 45h
	ACI 'T'   
	ACI XYZ
	SUB A  
	SUB B  
	SUB C  
	SUB D  
	SUB E  
	SUB H  
	SUB L  
	SUB M  
	SUI    low $
	SUI 4 
;	SUI 0x45   
	SUI 45h
	SUI 'T'   
	SUI XYZ
	SBB A  
	SBB B  
	SBB C  
	SBB D  
	SBB E  
	SBB H  
	SBB L  
	SBB M  
	SBI    high $
	SBI 4 
;	SBI 0x45   
	SBI 45h
	SBI 'T'   
	SBI XYZ
	INR A 
	INR B 
	INR C 
	INR D 
	INR E 
	INR H 
	INR L 
	INR M 
	DCR A 
	DCR B 
	DCR C 
	DCR D 
	DCR E 
	DCR H 
	DCR L 
	DCR M 
	INX BC  
	INX HL  
	INX DE  
	INX SP 
	DCX BC  
	DCX HL  
	DCX DE  
	DCX SP 
	DAD BC  
	DAD HL  
	DAD DE  
	DAD SP 
	DAA  
	ANA A  
	ANA B  
	ANA C  
	ANA D  
	ANA E  
	ANA H  
	ANA L  
	ANA M  
	ANI    low $
	ANI 4 
;	ANI 0x45   
	ANI 45h
	ANI 'T'   
	ANI XYZ
	ORA A 
	ORA B 
	ORA C 
	ORA D 
	ORA E 
	ORA H 
	ORA L 
	ORA M 
	XRA A  
	XRA B  
	XRA C  
	XRA D  
	XRA E  
	XRA H  
	XRA L  
	XRA M  
	XRI    high $
	XRI 4 
;	XRI 0x45   
	XRI 45h
	XRI 'T'   
	XRI XYZ
	CMP A   
	CMP B   
	CMP C   
	CMP D   
	CMP E   
	CMP H   
	CMP L   
	CMP M   
	CPI    low $
	CPI 4 
;	CPI 0x45   
	CPI 45h
	CPI 'T'   
	CPI XYZ
	RLC   
	RRC    
	RAL 
	RAR    
	CMA    
	CMC  
	STC   
;	JMP 0x1234
	JMP 1234h
	JMP TEST3 
;	JMP "xyay"
	JMP 'l' 
	JMP $
	JMP $+8
	JMP $-8
	JMP $+'T'
	JMP $+TEST3	
	JMP $-TEST3	
;	JNZ 0x1234
	JNZ 1234h
	JZ TEST3 
;	JNC "xyay"
	JC 'l' 
	JPO $
	JPE $+8
	JP $-8
	JM $+'T'
;	CALL 0x1234
	CALL 1234h
	CALL TEST3 
;	CALL "xyay"
	CALL 'l' 
	CALL $
	CALL $+8
	CALL $-8
	CALL $+'T'
	CALL $+TEST3	
;	CNZ 0x1234
	CNZ 1234h
	CZ TEST3 
;	CNC "xyay"
	CC 'l' 
	CPO $
	CPE $+8
	CP $-8
	CM $+'T'
	RET    
	RNZ 
	RZ 
	RNC 
	RC  
	RPO
	RPE 
	RP 
	RM 
;	RST 0x1234
	RST 7h
;	RST TEST3 
	RST XYZ 
;	RST "xyay"
;	RST 'l' 
;	RST $
;	RST $+8
;	RST $-8
;	RST $+'T'
	PCHL   
	PUSH DE  
	PUSH BC
	PUSH HL
	PUSH PSW
	POP  DE  
	POP  BC
	POP  HL
	POP  PSW
	XTHL   
	SPHL 
;	IN  0x1234
	IN  low 1234h
	IN  TEST3 
;	IN  "xyay"
	IN  'l' 
	IN  low $
	IN  low ($+8)
	IN  high ($-8)
	IN  low ($+'T')
;	OUT 0x1234
	OUT high 1234h
	OUT TEST3 
;	OUT "xyay"
	OUT 'l' 
	OUT low $
	OUT high ($+8)
	OUT low ($-8)
	OUT high ($+'T')
	EI    
	DI    
	HLT    
	NOP   


	END



