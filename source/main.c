#include <stdio.h>
#include <stdlib.h>
#include "arm_math.h"

//#define __WIKIPEDIA_TEST__ 0
//#define __MYCOURSES_TEST__ 1

#define S_DEF 4
#define V_DEF 3

#define N_OBS_TAKEN 20
#define RAND() (float)rand()/(float)(RAND_MAX)

// typedefs
typedef struct {
	int S;
	int V;
	float transition[S_DEF][S_DEF];
	float emission[S_DEF][V_DEF];
	float prior[S_DEF];
} hmm_desc;

// Assembly Functions
extern int ViterbiUpdate_asm(float* viterbi_in, float* viterbi_out, int obs, hmm_desc *model);
int ViterbiUpdate_c(float* viterbi_in, float* viterbi_out, int obs, hmm_desc *model);
int Viterbi_C(int* Observations, int Nobs, int* EstimatedStates, hmm_desc* hmm);

float states[N_OBS_TAKEN+1][2*S_DEF];

int main()
{	

	hmm_desc hmm1 = {
	S_DEF, 
	V_DEF,
//#ifdef __WIKIPEDIA_TEST__
//	{
//		{.7, .3},
//		{.4, .6}
//	},
//	{
//		{.5, .4, .1},
//		{.1, .3, .6}
//	},
//		{.6, .4}
//#elif __MYCOURSES_TEST__
	{
		{0.09f, 0.9f, 0.01f, 0.0f},
		{0.09f, 0.01f, 0.9f, 0.0f},
		{0.09f, 0.45f, 0.01f, 0.45f},
		{0.5f, 0.5f, 0.0f, 0.0f}
	},
	{
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 0.0f}
	},
		{0.25f, 0.25f, 0.25f, 0.25f}
//#endif
	};
	//This is the array of observations (same for all three hmm structs)
	//int vitTestArray[20] = {2,1,1,2,1,2,1,0,1,2,1,1,2,1,2,1,1,2,1,0};
	//The number of observations
	//int nobs = 20;
	// int obs[N_OBS_TAKEN] = {0, 1, 2};
	int obs[N_OBS_TAKEN] = {2,1,1,2,1,2,1,0,1,2,1,1,2,1,2,1,1,2,1,0};
	//float viterbi_in[2*S_DEF];
	int EstimatedStates[N_OBS_TAKEN];
	Viterbi_C(obs, N_OBS_TAKEN, EstimatedStates, &hmm1);
	/*
	for (int j = 1; j < N_OBS_TAKEN; j++) {
		//float viterbi_out[2*S_DEF];
		int s = ViterbiUpdate_asm(states[j-1], states[j], obs[j], &hmm1);
		
		for (int k = 0; k < S_DEF; k++) {
			viterbi_in[2*k] = viterbi_out[2*k], viterbi_in[2*k+1] = viterbi_out[2*k+1];
		}
		
		for (int i = 0; i < S_DEF; i++) { 
			printf("vit: %f\t psi: %f\n", states[j][2*i], states[j][2*i+1]);
		}
	}
	*/
	return 0;
}
int Viterbi_C(int* Observations, int Nobs, int* EstimatedStates, hmm_desc* hmm) {
	for (int i =0; i < S_DEF; i++) {
		states[0][2*i] = hmm->prior[i] * hmm->emission[i][Observations[0]];
		states[0][2*i+1] = 0;
		printf("%f = %f * %f\n", states[0][2*i], hmm->prior[i], hmm->emission[i][Observations[0]]);
	}
	for (int i = 1; i < Nobs+1; i++) {
		ViterbiUpdate_c(states[i-1], states[i], Observations[i], hmm);
		for (int j = 0; j < S_DEF; j++) { 
			printf("(%f, %f)\t", states[i][2*j], states[i][2*j+1]);
		}
		printf("\n");
	}
	
	float prob = -1;
	for (int i = 0; i < S_DEF; i++) {
		if (states[V_DEF][2*i] > prob) {
			prob = states[V_DEF][2*i];
			EstimatedStates[i] = (int) states[V_DEF][2*i+1];
		}
	}
	for (int i = 0; i < Nobs; i++) {
		printf("%d\t", EstimatedStates[i]);
	}
	printf("Max Prob: %3.3f\n", prob);
	return 0;
}
int ViterbiUpdate_c(float* viterbi_in, float* viterbi_out, int obs, hmm_desc *model) {
	int max_index = -1;
	float trans_p[S_DEF];
	for (int i = 0; i < model->S; i++) {
		
		for (int j = 0; j < model->S; j++) {
			trans_p[j] = viterbi_in[2*j] * model->transition[j][i];
		}
		float max = -1000;
		for (int j = 0; j < model->S; j++) {
			if (trans_p[j] > max) {
				max = trans_p[j];
				max_index = j;
			}
		}
		viterbi_out[2*i] = max;						// assign vit (value)
		//printf("%s: %3.3f\n", PRINT_STATE[i], max);
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
