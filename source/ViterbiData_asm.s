	AREA ViterbiData, CODE, READONLY
			
		export	vitpsi_in
		export	OBS
		export	S_DEF
		export	V_DEF
		export	emission
		export	transmission
		export	prior	
			
vitpsi_in 		DCFS		0.3, 0.0, 0.04, 0
OBS						DCD	1
S_DEF					DCD	2
V_DEF					DCD	3
transmission	DCFS 	0.7, 0.3, 0.4, 0.6
emission 			DCFS	0.5, 0.4, 0.1, 0.1, 0.3, 0.6
prior					DCFS	0.6, 0.4
		END
				
				