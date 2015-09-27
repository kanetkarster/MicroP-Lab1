		AREA ViterbiUpdate, CODE, READONLY
			EXPORT ViterbiUpdate_asm
ALIAS 
ViterbiUpdate_asm
;a0 A pointer to the vitpsi[(vit, psi),t-1] vector
;a1 A pointer to the vitpsi[:,t] vector (for output)
;a2 The observation 
;a3 A Pointer to the HMM variables (struct)
			;VLDR S1, [R0]
			VLDR.F32 S2, [R1]
			VLDR.F32 S5, =0	;initial max value
			VLDR.F32 S6, =0	;initial max state

			LDR 	R4, [R3], #4	;nStates
			LDR 	R5, [R3], #4 	;V obs
			LDR 	R6, [R3], #4	;Addr of addr of Trans Matrix			
			LDR		R7, [R3], #4	;Addr of addr of Emission matrix
			;LDR		R8, [R3], #4	;Addr of Prior Matrix
			
			MOV		R9, #0				;parameter for loop
			MOV		R10, #0				;parameter for multiplyLoop
			MOV 	R11, #0				;initialize trans_p
			
			MOV		R11, SP							;get stack pointer
			;SUB		SP, SP, R4,LSL #2	;move stack pointer
			LDR 	R8,	[R7]			;addr Emiss[0][]
			LDR		R12, [R6]			;addr trans[0][]
			;VLDR		S3, [R12]		;trans[0][0]
			PUSH	{R1}
			
			B			loop
loop											;loop 0 to S
			CMP		R9, R4
			BEQ		return
			B			trans_pLoop
returnMult
			ADD		R8, R8, R2, LSL #2		;Move R8 to correct addr of Obs
			VLDR.F32 S7, [R8]						;load emiss[s][obs]
			VMUL.F32 S5, S5, S7

			VSTR.F32 S5, [R1], #4			;update vit part of vitpsi
			VSTR.F32 S6, [R1], #4			;update psi part of vitpsi
			
			
			ADD		R12, R12, #4				;shift trans[s][] to trans[s+1][]
			ADD		R8, R8, #4					;shift emiss[s] to emiss[s+1]
			ADD 	R9, R9, #1					;iterate loop var
			
			B loop
return
			;R9 free
			;R10 free
			;R12 free
			;S3 free
			;S4 freeeeeeeeeeeeee
			POP		{R1}
			MOV	R9, #0
			VLDR.F32 S4, =0							;clr s4 for storing sum/C[t]
			B		sumLoop
sumLoop
			CMP R9, R4
			BEQ sumReturn
			VLDR.F32 S3, [R1], #4				;load vit[][t] and move addr
			ADD		R1, R1, #4						;move addr again
			
			VADD.F32 S4, S4, S3					;update sum
sumReturn
			VLDR.F32 S5, =1.0
			VDIV.F32 S4, S5, S4				;divide scaling

;TODO:
;vit[:,t] = c[t] * vit[:,t]


			BX 		LR

trans_pLoop
			CMP		R10, R4
			BEQ 	returnMult
			VLDR.F32	S1, [R0], #4				;load vit and move addr
			ADD		R0, R0, #4							;move addr once more			
			VLDR.F32	S3, [R12]						;load trans[s_0,s]
			VMUL.F32	S4, S1, S3					;multiply vit[:,t-1] and trans[:,s]
			
			VCMP.F32	S4, S5
			VMOVGE.F32	S5, S4						;max Prob
			VMOVGE.F32	S6, R10						;max State
			
			ADD		R12, R12, R4,LSL #2	;move R12 to addr trans[S_0+4]
			ADD		R10, R10, #1
			
			
			END
				
