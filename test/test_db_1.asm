



label_1
	nop

label_2
	nop


	db	label_2 - label_1
	db	label_2-label_1
	db	label_2 - label_1, label_2 - label_1
	db	label_2-label_1,label_2-label_1
	db	"Hello!"
	db	"Hello!", "Hello!"
	db	"Hello!", "Hello!", label_2 - label_1
	db	"Hello!","Hello!",label_2-label_1
label_3
	db	"Hello!","Hello!",label_2-label_1
label_4:
	db	"Hello!","Hello!",label_2-label_1
label5
	db	"Hello!","Hello!",label_2-label_1
	db	$, 1







