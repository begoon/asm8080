

;data_neg	equ	-1
;data_neg	equ	(-1)
;data_neg	equ	0-1
;data_neg	equ	((-1))
;data_neg	equ	(((-1)))
data_neg	equ	((((-1))))


	org	0


	if 0

	mvi	a, -1
	mvi	b, -2
	
	lxi	b, -1
	lxi	d, -2

	endif


	mvi	h, data_neg





	end


