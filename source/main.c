#include <stdio.h>
#include <stdlib.h>
#include "arm_math.h"

#define N_STATES 2
#define N_OBS 1

#define RAND() (float)rand()/(float)(RAND_MAX)

// typedefs
typedef struct {
	int S;
	int V;
	float transition[N_STATES][N_STATES];
	float emission[N_STATES][N_OBS];
	float prior[N_STATES];
} HMM;

// Assembly Functions
extern int ViterbiUpdate_asm(float* viterbi_in, float* viterbi_out, float obs, HMM *model);
int ViterbiUpdate_c(float* viterbi_in, float* viterbi_out, int obs, HMM *model);
int Viterbi_C(int* Observations, int Nobs, int* EstimatedStates, HMM* hmm);

int main()
{	
	// output
	int EstimatedStates[N_OBS];
	// initial values for HMM
	float EMMISSION[N_STATES][N_OBS]= {1.0f, 2.0f};
	float TRANSMISSION[N_STATES][N_STATES]= {{1.0f, 2.0f},
																					 {31.0f, 4.0f}};
	float PRIOR[N_STATES] = {.25, .5};
	int Observations[N_OBS] = {1};
	
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
	states[0][1] = hmm->prior[1];
	
	for (int i = 0; i < Nobs; i++) {
		ViterbiUpdate_c(states[i], states[i+1], Observations[i], hmm);
	}
	
	float prob = -1;
	for (int i = 0; i < N_STATES; i++) {
		if (states[N_OBS][2*i] > prob) {
			prob = states[N_OBS][2*i];
			EstimatedStates[i] = (int) states[N_OBS][2*i+1];
		}
	}
	//printf("Ends at:\n");
	return 0;
}
int ViterbiUpdate_c(float* viterbi_in, float* viterbi_out, int obs, HMM *model) {
	float trans_p[N_STATES];
	float max = -1000;
	int max_index = -1;
	for (int i = 0; i < model->S; i++) {
		for (int j = 0; j < model->S; j++) {
			trans_p[j] = viterbi_in[j] * model->transition[i][j];
		}
		for (int j = 0; j < model->S; j++) {
			if (trans_p[j] > max) {
				max = trans_p[j];
				max_index = j;
			}
		}
		viterbi_out[2*i] = max;						// assign vit (value)
		viterbi_out[2*i + 1] = max_index; // assign psi (index)
		viterbi_out[2*i] = max * obs;
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
