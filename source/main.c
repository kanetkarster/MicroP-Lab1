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
int main()
{	
	// VITPSI
	float vitpsi_i[2*N_STATES];
	float vitpsi_o[2*N_STATES];
	
	float EMMISSION[N_STATES][N_OBS]= {1.0f, 2.0f};
	float TRANSMISSION[N_STATES][N_STATES]= {{1.0f, 2.0f},
																					 {3.0f, 4.0f}};
	float OBS = 2;
																					 
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
		vitpsi_i[2*i+1] = .5;
	}
	

	int s = ViterbiUpdate_c(vitpsi_i, vitpsi_o, OBS, &model);
	
	for (int i = 0; i < N_STATES; i++) {
		printf("psi: %3.3f\t vit %3.3f\n", vitpsi_o[2*i], vitpsi_o[2*i+1]);
	}
	
	printf("%d\n", s);
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
