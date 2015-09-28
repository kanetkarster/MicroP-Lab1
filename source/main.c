#include <stdio.h>
#include <stdlib.h>
#include "arm_math.h"

#define N_STATES 2
#define N_OBS 1
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
	// VITPSI
	float* vitpsi_i = malloc(2*N_STATES*sizeof(float));
	float* vitpsi_o = malloc(2*N_STATES*sizeof(float));
	
	float EMMISSION[N_STATES][N_OBS]= {1.0f, 2.0f};
	float TRANSMISSION[N_STATES][N_STATES]= {{1.0f, 2.0f},
																					 {3.0f, 4.0f}};
	float OBS = 2;
	// Hidden Markov Model
	HMM model;
	model.S = N_STATES;
	model.V = N_OBS;
	model.transition = malloc(N_STATES * sizeof(float*));
	model.emission = malloc(N_STATES * sizeof(float*));
	model.prior = malloc(N_STATES * sizeof(float*));
	
	for (int i=0; i < N_STATES; i++) {
		model.transition[i] = malloc(N_STATES * sizeof(float*));
		model.emission[i] = malloc(N_OBS * sizeof(float*));
	}
	
	for (int i = 0; i < N_STATES; i++) {
		for (int j = 0; j < N_STATES; j++) {
			model.transition[i][j] = TRANSMISSION[i][j];
		}
		for (int j = 0; j < N_OBS; j++) {
			model.emission[i][j] = EMMISSION[i][j];
		}
		vitpsi_i[i] = 0;
		vitpsi_i[i+1] = 0;
	}
	

	int s = ViterbiUpdate_c(vitpsi_i, vitpsi_o, OBS, &model);
	
	for (int i = 0; i < N_STATES; i++) {
		printf("psi: %3.3f\t vit %3.3f\n", vitpsi_o[i], vitpsi_o[i+1]);
	}
	
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
