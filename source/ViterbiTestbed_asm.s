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
		PUSH	{R4-R11}
		LDR		R0, =vitpsi_in
		LDR		R1, =vitpsi_out
		LDR		R2, OBS
		LDR		R3, =S_DEF
		
		B		ViterbiUpdate_asm
		BX		LR
	
		POP		{R4-R11}
	
		END