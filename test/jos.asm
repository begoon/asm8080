; test cases from jos
        NOP
        NOP
        NOP
LBL     
	ORG     0100H   ;LBL should have value 3
        NOP
        LXI     B,LBL
        NOP
;
;
; Intel 8080-8085 Assembly Language Programming 1977.
;
; Page 2.5

HERE:   MVI C,0BAH
ABC:    MVI E,15
        MVI E,15D
LABEL:  MVI A,72Q
NOW:    MVI D,11110110B
JOS:    MVI A,72O
JOS1:   lxi	h,177777O

DATE:   DB  'TODAY','S DATE'
; this on is added  for my interest
;
XYZZY::	DB	0	; 

;
; Intel 8080-8085 Assembly Language Programming 1977.
;
; Page 2.12
;

; All should generate 65D/41H
; NOTE.  asm8080 is not good at expression hierarchy so, brase boys brase.
        MVI B,      5+(30*2)		; force presidence
        MVI B,      (25/5)+(30*2)	; force presidence
        MVI B,      5+((-30)*(-2))	; needed or it thinks its a subtract.
        MVI B,      (25 MOD 8) + 64

;;;;;;

NUM1    EQU     00010001B
NUM2    EQU     00010000B
	DB      NOT NUM1
	DB      NUM1 AND NUM2
	DB      NUM1 AND NUM1
	DB      NUM1 OR NUM2
	DB      NUM2 OR NUM2
	DB      NUM1 XOR NUM2
	DB      NUM1 XOR NUM1
	DB      5 EQ 5
	DB      5 NE 5
	DB      5 GT 4
	DB      4 GE 5
	DB      4 LT 5
	DB      4 LE 5

	end
