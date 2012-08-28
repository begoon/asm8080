

	org	1234h


start

	if 1

;	mvi	a, LOW(start)
	mvi	a, start and 0FFh

;	mvi	a, HIGH(start)
	mvi	a, start >> 8
	mvi	a, start / 256

;	mvi	a, NOT(LOW(start))
	mvi	a, 0FFh - (start and 00FFh)

;	mvi	a, LOW((NOT)start)
	mvi	a, (0FFFFh - start) and 0FFh

	endif

;	mvi	a, NOT(HIGH(start))
	mvi	a, 0FFh - (start >> 8)
	mvi	a, 0FFh - (start / 256)
	mvi	a, 0FFh - (1234h / 256)

;	mvi	a, HIGH((NOT)start)
	mvi	a, (0FFFFh - start) >> 8
	mvi	a, (0FFFFh - start) / 256



