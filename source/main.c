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
int ViterbiUpdate_c(float* viterbi_in, float* viterbi_out, int obs, HMM *model);
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
	int s = ViterbiUpdate_c(vitpsi_i, vitpsi_o, OBS, &model);
	
	printf("%d\n", s);
	return 0;
}

int ViterbiUpdate_c(float* viterbi_in, float* viterbi_out, int obs, HMM *model) {
	float max = -10000;
	int max_index;
	float sum = 0;
	for (int s = 0; s < model->S; s++) {
		for (int j = 0; j < 2*model->S; j++) {
			float* transmitted = model->transition[j] + j*sizeof(&model->transition);
			float val = viterbi_in[j] * *transmitted;
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
