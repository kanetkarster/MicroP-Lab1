		AREA ViterbiUpdate, CODE, READONLY
			EXPORT ViterbiUpdate_asm
		
ViterbiUpdate_asm
;a0 A pointer to the vitpsi[:,t-1] vector
;a1 A pointer to the vitpsi[:,t] vector (for output)
;a2 The observation 
;a3 A Pointer to the HMM variables (struct)
			VLDR S1, [R0]
			VLDR S2, [R1]


			LDR 	R4, [R3], #4	;S states
			LDR 	R5, [R3], #4 	;V obs
			LDR 	R6, [R3], #4	;Addr of addr of Trans Matrix			
			LDR		R7, [R3], #4	;Addr of addr of Emission matrix
			LDR		R8, [R3], #4	;Addr of Prior Matrix
			
			MOV	R9, #0
			B			loop
				
loop
			
			CMP	R9, R4
			BNE		loop
			
			
			
			BX LR ;
			
			END
				
