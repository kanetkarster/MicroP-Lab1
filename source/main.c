#include <stdio.h>
#include <stdlib.h>
#include "arm_math.h"

// typedefs
typedef struct {
	int S;
	int V;
	float** transition;
	float** emission;
	float* prior;
} HMM;

// Assembly Functions
extern int ViterbiUpdate_asm(float* viterbi_in, float* viterbi_out, float obs, HMM *model);
int ViterbiUpdate_c(float* viterbi_in, float* viterbi_out, float obs, HMM *model);
int main()
{	
	int S = 4, V = 10;
	// VITPSI
	float* vitpsi_i = malloc(2*S*sizeof(float));
	float* vitpsi_o = malloc(2*S*sizeof(float));
	
	float OBS = 2;
	
	// Hidden Markov Model
	HMM model;
	model.S = S;
	model.V = V;
	model.transition = malloc(S * S * sizeof(float));

	model.emission = malloc(S * V * sizeof(float));
	model.prior = malloc(S * sizeof(float));

	int i, j;
	for (i=0; i < S; i++) {
		for (j=0; j < S; j++) {
			model.transition[i][j] = (float)rand()/(float)(RAND_MAX);
		}
		for (j=0; j < V; j++) {
			model.emission[i][j] = (float)rand()/(float)(RAND_MAX);
		}
		model.prior[i] = (float)rand()/(float)(RAND_MAX);
	}
	int s = ViterbiUpdate_asm(vitpsi_i, vitpsi_o, OBS, &model);
	
	printf("%d\n", s);
	return 0;
}

int ViterbiUpdate_c(float* viterbi_in, float* viterbi_out, float obs, HMM *model) {
	return 0;
}
