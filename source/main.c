#include <stdio.h>
#include <stdlib.h>
#include "arm_math.h"

#define N_STATES 2
#define N_OBS 3

#define RAND() (float)rand()/(float)(RAND_MAX)

static char *PRINT_STATE[2] = {"HEALTHY",
															 "FEVER"};

// typedefs
typedef struct {
	int S;
	int V;
	float transition[N_STATES][N_STATES];
	float emission[N_STATES][N_OBS];
	float prior[N_STATES];
} HMM;

// Assembly Functions
extern int ViterbiUpdate_asm(float* viterbi_in, float* viterbi_out, int obs, HMM *model);
int ViterbiUpdate_c(float* viterbi_in, float* viterbi_out, int obs, HMM *model);
int Viterbi_C(int* Observations, int Nobs, int* EstimatedStates, HMM* hmm);

int main()
{	

	// output
	int EstimatedStates[N_OBS];
	// initial values for HMM
	float EMMISSION[N_STATES][N_OBS]= {{0.5f, 0.4f, 0.1f},
																		 {0.1f, 0.3f, 0.6f}};

	float TRANSMISSION[N_STATES][N_STATES]= {{0.7f, 0.3f},
																					 {0.4f, 0.6f}};
	float PRIOR[N_STATES] = {.6, .4};
	int Observations[N_OBS] = {0, 1, 2};
	// Fill Hidden Markov Model

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
		model.prior[i] = PRIOR[i];
	}
	
	int s = Viterbi_C(Observations, N_OBS, EstimatedStates, &model);
	
	for (int i = 0; i < N_OBS; i++) {
		printf("Observed State: %d\n", EstimatedStates[i]);
	}
	
	return 0;
}
int Viterbi_C(int* Observations, int Nobs, int* EstimatedStates, HMM* hmm) {
	float states[Nobs+1][2*N_STATES];
	states[0][0] = hmm->prior[0];
	states[0][1] = 0;
	states[0][2] = hmm->prior[1];
	states[0][3] = 1;
	
	for (int i = 0; i < Nobs; i++) {
		ViterbiUpdate_asm(states[i], states[i+1], Observations[i], hmm);
		for (int j = 0; j < N_STATES; j++) {
			printf("vit: %f\tpsi: %f\n", states[i+1][2*j]);
		}
	}
	
	float prob = -1;
	for (int i = 0; i < N_STATES; i++) {
		if (states[N_OBS][2*i] > prob) {
			prob = states[N_OBS][2*i];
			EstimatedStates[i] = (int) states[N_OBS][2*i+1];
		}
	}
	printf("Max Prob: %3.3f\n", prob);
	return 0;
}
int ViterbiUpdate_c(float* viterbi_in, float* viterbi_out, int obs, HMM *model) {
	int max_index = -1;
	float trans_p[N_STATES];
	for (int i = 0; i < model->S; i++) {
		for (int j = 0; j < model->S; j++) {
			trans_p[j] = viterbi_in[2*j] * model->transition[i][j];
		}
		float max = -1000;
		for (int j = 0; j < model->S; j++) {
			if (trans_p[j] > max) {
				max = trans_p[j];
				max_index = j;
			}
		}
		viterbi_out[2*i] = max;						// assign vit (value)
		printf("%s: %3.3f\n", PRINT_STATE[i], max);
		viterbi_out[2*i + 1] = max_index; // assign psi (index)
		viterbi_out[2*i] = max * model->emission[i][obs];
	}
	
	float sum = 0;
	for (int i = 0; i < model->S; i++) {
		sum += viterbi_out[2*i];
	}
	
	float scale = 1.0f/sum;
	
	for (int i = 0; i < model->S; i++) {
		viterbi_out[2*i] = viterbi_out[2*i] * scale;
	}
	
	return 0;
}
