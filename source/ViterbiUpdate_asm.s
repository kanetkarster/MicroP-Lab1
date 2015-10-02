		AREA ViterbiUpdate, CODE, READONLY
			EXPORT ViterbiUpdate_asm
p_vitpsiIn	RN R0
p_vitpsiOut RN R1
obs					RN R2
p_HMM				RN R3
nStates			RN R4
p_trans			RN R5
p_emiss			RN R6
max_state 	RN R7
cnt					RN R9
cntIn				RN R10
update			RN R11

vitpsiIn		SN S1
vitpsiOut		SN S2
trans				SN S3
max_prob 		SN S5
emiss				SN S7

ViterbiUpdate_asm
			PUSH {R4-R11}			
			VLDR.F32 vitpsiOut, [p_vitpsiOut]				;initally load vitpsiOut
			VLDR.F32 max_prob, =0										;initial max value
			MOV		max_state, #0											;initial max state

			LDR 	nStates, [p_HMM], #8							;nStates
			MOV		p_trans, p_HMM										;addr of Transmission matrix
			MOV		p_emiss, p_HMM										;addr of Emission matrix
			
			MUL		R8, nStates, nStates							;get S*S
			ADD		p_emiss, p_emiss, R8, LSL #2			;shift to addr of emission
			SUB		p_HMM, p_HMM, #4									;update p_HMM
			MOV		R8, #0
			LDR		R8, [p_HMM]												;load the value of V
			
			MOV		cnt, #0														;parameter for loop
			MOV		cntIn, #0													;parameter for multiplyLoop
			PUSH	{p_vitpsiOut}											;preserve addr of vitpsiOut		
loop																					;loop 0 to nStates
			VLDR.F32	max_prob, =-1000							;get a max value for initial comparisons in Loop
			MOV		cntIn, #0													;make sure cnt for innerloop in set to inital value of 0
			CMP		cnt, nStates
			BEQ		returnLoop
			PUSH	{p_vitpsiIn}											;preserve addresses
			PUSH	{p_trans}
			PUSH	{p_emiss}
			MUL		update, R8, cnt										
			ADD		p_emiss, p_emiss, update,LSL #2		;update emission
trans_pLoop																		;nested loop for transp math
			CMP		cntIn, nStates
			BEQ 	returnTrans
			VLDR.F32	vitpsiIn, [p_vitpsiIn], #4		;load vit and move addr
			ADD		p_vitpsiIn, p_vitpsiIn, #4				;move addr once more			
			VLDR.F32	trans, [p_trans]							;load trans[cntIn,cnt]
			VMUL.F32	S4, vitpsiIn, trans						;multiply vit[:,t-1] and trans[:,s]
			
			VCMP.F32	S4, max_prob
			VMRS.F32	APSR_nzcv, FPSCR
			VMOVGE.F32	max_prob, S4								;max max Prob
			MOVGE		max_state, cntIn								;update max state

			ADD		p_trans, p_trans, nStates, LSL #2	;shift trans[cntIn][] to trans[cntIn+1][]
			ADD		cntIn, cntIn, #1									;update count 
			B			trans_pLoop
returnTrans
			ADD		p_emiss, p_emiss, obs, LSL #2			;update emission
			VLDR.F32 emiss, [p_emiss]								;load emiss[cnt][0]
			VMUL.F32 max_prob, max_prob, emiss			;update max_prob

			VSTR.F32 max_prob, [p_vitpsiOut]				;update vit part of vitpsiOut
			ADD		p_vitpsiOut, p_vitpsiOut, #4			;update addr of vitpsi_out
			VMOV  S6, max_state
			VCVT.F32.S32	S6, S6
			VSTR.F32 S6, [p_vitpsiOut]							;update psi part of vitpsiOut
			ADD		p_vitpsiOut, p_vitpsiOut, #4			;update addr of vitpsi_out
						
			ADD 	cnt, cnt, #1											;iterate loop var
			POP		{p_emiss}													;restore addresses that were preserved
			POP		{p_trans}
			ADD		p_trans, p_trans, #4							;move addr_trans to addr trans[S_0+4]
			POP		{p_vitpsiIn}
			B 		loop
returnLoop
			POP		{p_vitpsiOut}											;restore address that were preserved
			MOV		cnt, #0															;reset loop/count var
			VLDR.F32 S4, =0													;clr s4 for storing sum/C[t]
			PUSH	{p_vitpsiOut}										;preserve addr
sumLoop
			CMP 	cnt, nStates
			BEQ 	sumReturn
			VLDR.F32 vitpsiOut, [p_vitpsiOut], #4		;load vit[cnt][t] and move addr
			ADD		p_vitpsiOut, p_vitpsiOut, #4			;move addr again
			VADD.F32 S4, S4, vitpsiOut							;update sum
			ADD		cnt, cnt, #1
			B			sumLoop
sumReturn
			VLDR.F32 S8, =1.0
			VDIV.F32 S4, S8, S4											;divide for scaling factor
			POP		{p_vitpsiOut}											;restore addr for vitpsiOut
			MOV		cnt, #0														;reset counter
			PUSH	{p_vitpsiOut}
updateVitLoop
			CMP		cnt, nStates
			BEQ		updateReturn
			VLDR.F32	vitpsiOut, [p_vitpsiOut]			;load vit[cnt][t]
			VMUL.F32	vitpsiOut, vitpsiOut, S4			;update the value of vit[cnt][t]
			VSTR.F32	vitpsiOut, [p_vitpsiOut]			;store back into the vitspe vector
			ADD		p_vitpsiOut, p_vitpsiOut, #8			;shift over 2 spot for next vit entry
			ADD		cnt, cnt, #1
			B			updateVitLoop
updateReturn
			POP		{p_vitpsiOut}											;restore addr for return
			POP		{R4-R11}
			BX 		LR																;exit
			NOP
					
			END