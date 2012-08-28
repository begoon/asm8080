


	if 0
_HIGH	equ	start / 256
_LOW	equ	start & 0FFh
	endif


	org	0


	ds	258


start

_HIGH	equ	start / 256
_LOW	equ	start & 0FFh

;	mvi	a, start
;	mvi	a, start >> 8
;	mvi	a, (start >> 8)
;	mvi	a, (start / 256)
	mvi	a, start / 256
;	mvi	a, (1 / 1)
	mvi	a, _LOW
	mvi	a, _HIGH



	end



