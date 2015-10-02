#include <stdio.h>
#include <stdlib.h>
#include "arm_math.h"

#define S_DEF 4
#define V_DEF 3
#define N_OBS_TAKEN 20

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
float EstimatedStates[N_OBS_TAKEN];
float accObs[45] = {-0.47, 0.51, 0.69, 0.02, 0.54, 0.12, -0.48, 0.42, 3.73, 
										6.19, 6.74, 6.49, -5.71, -4.15, -7.35, -7.20, -7.02, -3.11,
										4.37, 2.17, 4.36, 1.86, -0.62, 0.49, 0.08, 0.60, 0.27, 
										0.12, 6.67, 7.13, 7.92, 0.80, -1.77, -3.59, -0.87, 4.60, 
										4.25, 6.57, 2.44, 0.03, 0.56, 0.10, 0.48, 0.33, 0.12};
int acceleratometer_observations[9];
int acceleratometer_states[9];
int ReadAccelerometer(float* data, int data_len, int* observation, int* nObs) {
	float alpha1 = 0.7;
	float alpha2 = 0.8;
	int N[] = {3, 3, 3};
	int ct[3] = {0, 0, 0};
	int last_entry = -1;
	int current_state = 0;
	*nObs = 0;
	int z;
	for (int i=0; i < data_len; i++) {
		if (-1*alpha1 < data[i] && data[i] < alpha1) {
			z = 0;
		} 
		else if ( data[i] > alpha2) {
			z = 1;
		} 
		else if ( data[i] < -1 * alpha2) {
			z = 2;
		} 
		else z = -1;
		if (z != -1) {
			if (z != current_state) {
				ct[current_state] = 0;
				current_state = z;
			}
			ct[z]++;
			if (ct[z] >= N[z] && z != last_entry) {
				observation[*nObs] = z;
				last_entry = z;
				*nObs++;
				printf("%d\t", last_entry);
			}
		}
	}
	printf("\n");
	return 0;
}
										
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
	printf("ESTIMATED STATES TEST 1\n");
	printf("=======================\n");
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

	int nobs;
	printf("OBSERVATIONS FROM ACCELEROMETER\n");
	printf("===============================\n");
	ReadAccelerometer(accObs, 45, acceleratometer_observations, &nobs);
	printf("ESTIMATED STATES TEST 2\n");
	printf("=======================\n");
	Viterbi_C(acceleratometer_observations, 9, EstimatedStates, &hmm1);
	return 0;
}
int Viterbi_C(int* Observations, int Nobs, int* EstimatedStates, hmm_desc* hmm) {
	for (int i =0; i < S_DEF; i++) {
		states[0][2*i] = hmm->prior[i] * hmm->emission[i][Observations[0]];
		states[0][2*i+1] = 0;
		//printf("%f = %f * %f\n", states[0][2*i], hmm->prior[i], hmm->emission[i][Observations[0]]);
	}
	for (int i = 1; i < Nobs+1; i++) {
		ViterbiUpdate_asm(states[i-1], states[i], Observations[i], hmm);
//		for (int j = 0; j < S_DEF; j++) { 
//			printf("(%f, %f)\t", states[i][2*j], states[i][2*j+1]);
//		}
//		printf("\n");
	}
	for (int i = 1; i < Nobs+1; i++) {
		float max = -1;
		for (int j = 0; j < S_DEF; j++) {
			if (states[i][2*j] > max) {
				max = states[i][2*j];
				EstimatedStates[i-1] = (int) states[i][2*j+1];
				//printf("(%f, %f)\t", states[i][2*j], states[i][2*j+1]);
			}
		}
		//printf("\n");
	}
	for (int i = 0; i < Nobs; i++) {
		printf("%d\t", EstimatedStates[i]);
	}
	printf("\n");
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
