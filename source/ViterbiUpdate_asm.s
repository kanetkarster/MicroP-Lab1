	AREA ViterbiUpdate, CODE, READONLY
	EXPORT 	ViterbiUpdate_asm

ViterbiUpdate_asm	
	; load S (n states) into R5
	LDR R4, [R0], #4	;Load S
	LDR R5, [R0], #4	;Load V
	LDR R6, [R0], #4	;Load transmission	(**)
	LDR R7, [R0], #4	;Load emmission		(**)
	LDR R8, [R0], #4	;Load prior			(*)
	
	; Loop from 0 -> S
	
	BX LR
	END

