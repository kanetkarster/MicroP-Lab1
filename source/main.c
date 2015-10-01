#include <stdio.h>
#include <stdlib.h>
#include "arm_math.h"

#define S_DEF 2
#define V_DEF 3
#define N_OBS_TAKEN 3

typedef struct {
	int S;
	int V;
	float transition[S_DEF][S_DEF];
	float emission[S_DEF][V_DEF];
	float prior[S_DEF];
} hmm_desc;

// Assembly Functions
extern int ViterbiUpdate_asm(float* viterbi_in, float* viterbi_out, int obs,  hmm_desc *model);
int ViterbiUpdate_c(float* viterbi_in, float* viterbi_out, int obs,  hmm_desc *model);
int main()
{	
	int i;
	//This is one of the three hmm structs that you will be provided
	hmm_desc hmm1 = {
	S_DEF, 
	V_DEF,
	{
		{.7, .3},
		{.4, .6}
	},
	{
		{.5, .4, .1},
		{.1, .3, .6}
	},
		{.6, .4}
	};
	//This is the array of observations (same for all three hmm structs)
	//int vitTestArray[20] = {2,1,1,2,1,2,1,0,1,2,1,1,2,1,2,1,1,2,1,0};
	//The number of observations
	//int nobs = 20;
	int obs[3] = {0, 1, 2};
	float viterbi_in[2*S_DEF];

	for (i=0; i < S_DEF; i++) {
		viterbi_in[2*i] = hmm1.prior[i] * hmm1.emission[i][obs[0]];
		viterbi_in[2*i + 1] = 0;
	}
int loop = 0;
for (int j=1; j < N_OBS_TAKEN; j++) {
	float viterbi_out[2*S_DEF];
	int s = ViterbiUpdate_asm(viterbi_in, viterbi_out, obs[j], &hmm1);
	for (int k = 0; k < S_DEF; k++){
		viterbi_in[2*k] = viterbi_out[2*k], viterbi_in[2*k+1] = viterbi_out[2*k+1];
	
	}

	//float temp[2*S_DEF];
	//for (int j = 0; j < 2*N_STATES; j++) {
	//	temp[j] = vitpsi_o[j];
	//}
	for (int i = 0; i < S_DEF; i++) { 
		printf("vit: %f\t psi: %f\n", viterbi_out[2*i], viterbi_out[2*i+1]);
	}
}
	
	return 0;
}

int ViterbiUpdate_c(float* viterbi_in, float* viterbi_out, int obs, hmm_desc *model) {
	float max = -10000;
	int max_index;
	float sum = 0;
	for (int s = 0; s < model->S; s++) {
		for (int j = 0; j < 2*model->S; j++) {
			float val = viterbi_in[j] * model->transition[j][s];
			sum += val;
			if (val > max) {
				max = val;
				max_index = j;
			}
		}
		// fill vit
		viterbi_out[2*s] = max;
		// fill psi
		viterbi_out[2*s+1] = ((float)max_index ) * model->emission[s][obs];
	}
	float scale = 1.0f / sum;
	for (int s = 0; s < model->S; s++) {
		viterbi_out[2*s] = scale * viterbi_out[s];
	}
	
	return 0;
}
