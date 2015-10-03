	;Test bench for ViterbiUpdate_asm
	;Please refer to ViterbiData and ViterbiOut for code/data varibales
		
		AREA ViterbiTestbed, CODE, READONLY
			EXPORT ViterbiTestbed_asm
			IMPORT ViterbiUpdate_asm
				
		import 	vitpsi_in
		import	vitpsi_out
		import	OBS
		import	S_DEF
		import	V_DEF
		import	emission
		import	transmission
		import	prior	
			

ViterbiTestbed_asm
		PUSH	{R4-R11}								;preserve the registers
		LDR		R0, =vitpsi_in					;load the addr of viterbi_in
		LDR		R1, =vitpsi_out					;load the addr of viterbi_out
		LDR		R2, OBS									;load the observation
		LDR		R3, =S_DEF							;load the pointer to the struct
		
		B			ViterbiUpdate_asm				;branch to update
		POP		{R4-R11}								;restore the registers
		BX		LR
	
		
	
		END