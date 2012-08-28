

;******************************************************************************


DUMMY	equ	0


;	org	0000H


start:	lxi	h, btbl		;Registers H and L will
				;Point to branch table.

gtbit:	rar
	jc	getad
	inx	h
	inx	h

	jmp	gtbit
;	jmp	toto
getad:	mov	e, m
	inx	h

	mov	d, m
	xchg

	pchl



btbl:	dw	rout1
	dw	rout2
	dw	rout3
	dw	rout4
	dw	rout5
	dw	rout6
	dw	rout7
	dw	rout8



rout1:
rout2:
rout3:
rout4:
rout5:
rout6:
rout7:
rout8:
	jmp	start




;	end




