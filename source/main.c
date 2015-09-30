#include <stdio.h>
#include <stdlib.h>
#include "arm_math.h"

#define N_STATES 2
#define N_OBS 3

#define RAND() (float)rand()/(float)(RAND_MAX)

// typedefs
typedef struct {
	int S;
	int V;
	float transition[N_STATES][N_STATES];
	float emission[N_STATES][N_OBS];
	float* prior;
} HMM;

// Assembly Functions
extern int ViterbiUpdate_asm(float* viterbi_in, float* viterbi_out, int obs, HMM *model);
int ViterbiUpdate_c(float* viterbi_in, float* viterbi_out, int obs, HMM *model);
int main()
{	
	// VITPSI
	float vitpsi_i[2*N_STATES];
	float vitpsi_o[2*N_STATES];
	
	float EMMISSION[N_STATES][N_OBS]= {1.0f, 2.0f};
	float TRANSMISSION[N_STATES][N_STATES]= {{1.0f, 2.0f},
																					 {3.0f, 4.0f}};

	int OBS = 0;

	// Hidden Markov Model
	HMM model;
	model.S = N_STATES;
	model.V = N_OBS;
	for (int i = 0; i < N_STATES; i++) {
		for (int j = 0; j < N_STATES; j++) {
			model.transition[i][j] = TRANSMISSION[i][j];
		}
		for (int j = 0; j < N_OBS; j++) {
			model.emission[i][j] = EMMISSION[i][j];
		}

		vitpsi_i[2*i] = .5;
		vitpsi_i[2*i+1] = .25;

	}
	

	int s = ViterbiUpdate_asm(vitpsi_i, vitpsi_o, OBS, &model);
	float temp[2*N_STATES];
	//for (int j = 0; j < 2*N_STATES; j++) {
	//	temp[j] = vitpsi_o[j];
	//}
	for (int i = 0; i < N_STATES; i++) { 
		printf("vit: %f\t psi: %d\n", vitpsi_o[2*i], vitpsi_o[2*i+1]);
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
