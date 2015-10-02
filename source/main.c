#include <stdio.h>
#include <stdlib.h>
#include "arm_math.h"
#include "test.h"


// Assembly Functions
extern int ViterbiUpdate_asm(float* viterbi_in, float* viterbi_out, int obs, hmm_desc *model);
int ViterbiUpdate_c(float* viterbi_in, float* viterbi_out, int obs, hmm_desc *model);
int Viterbi_C(int* Observations, int Nobs, int* EstimatedStates, hmm_desc* hmm);

float states[N_OBS_TAKEN+1][2*S_DEF];
float EstimatedStates[N_OBS_TAKEN];
int acceleratometer_observations[9];
int acceleratometer_states[9];
										
/*!
	ReadAccelerometer takes in an array of accelerometer data and gets observation
	values based off the accelerometer readings
										
	\param data
	\param data_len size of data
	\param observation output vector containing observed data.
	\param nObs	size of observation	
*/										
int ReadAccelerometer(float* data, int data_len, int* observation, int* nObs) {
	int ct[3] = {0, 0, 0};
	int last_entry = -1;
	int current_state = 0;
	*nObs = 0;
	int z;
	for (int i=0; i < data_len; i++) {
		// Determine current state
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
			// resets count vector if new observation
			if (z != current_state) {
				ct[current_state] = 0;
				current_state = z;
			}
			ct[z]++;
			// adds observation if more than N entries
			if (ct[z] >= N[z] && z != last_entry) {
				observation[*nObs] = z;
				last_entry = z;
				(*nObs)++;
				printf("%d\t", last_entry);
			}
		}
	}
	printf("\n");
	return 0;
}
/*!
	Calls tests in order:
1. Test 1
2. ReadAccelerometer
3. Test 2
*/
int main()
{	
	/*
	hmm_desc hmm1 = {
	S_DEF, 
	V_DEF,
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
	};
	*/
	//This is the array of observations (same for all three hmm structs)
	//int vitTestArray[20] = {2,1,1,2,1,2,1,0,1,2,1,1,2,1,2,1,1,2,1,0};
	//The number of observations
	// int obs[N_OBS_TAKEN] = {0, 1, 2};
	//int obs[N_OBS_TAKEN] = {2,1,1,2,1,2,1,0,1,2,1,1,2,1,2,1,1,2,1,0};
	//float viterbi_in[2*S_DEF];
	int EstimatedStates[N_OBS_TAKEN];
//	printf("ESTIMATED STATES TEST 1\n");
//	printf("=======================\n");
//	Viterbi_C(obs, N_OBS_TAKEN, EstimatedStates, &hmm3);
//	
/*
	int accel_obs;
	printf("OBSERVATIONS FROM ACCELEROMETER\n");
	printf("===============================\n");
	ReadAccelerometer(accObs, 45, acceleratometer_observations, &accel_obs);
	printf("ESTIMATED STATES TEST 2\n");
	printf("=======================\n");
	Viterbi_C(acceleratometer_observations, accel_obs, EstimatedStates, &hmm1);
*/
  float viterbi_test1[2*S_DEF];
	printf("Viterbi Update 1\n");
	printf("================\n");
	ViterbiUpdate_c(InputArray_1, viterbi_test1, Observation_1, &hmm1);
	for (int i = 0; i < S_DEF; i++) {
		printf("(%f, %f)\t", viterbi_test1[2*i], viterbi_test1[2*i+1]);
	}
//	printf("\nViterbi Update 2\n");
//	printf("================\n");
//	ViterbiUpdate_c(InputArray_2, viterbi_test1, Observation_1, &hmm2);
//	for (int i = 0; i < S_DEF; i++) {
//		printf("(%f, %f)\t", viterbi_test1[2*i], viterbi_test1[2*i+1]);
//	}
//	printf("\nViterbi Update 3\n");
//	printf("================\n");
//	ViterbiUpdate_c(InputArray_3, viterbi_test1, Observation_1, &hmm3);
//	for (int i = 0; i < S_DEF; i++) {
//		printf("(%f, %f)\t", viterbi_test1[2*i], viterbi_test1[2*i+1]);
//	}
	return 0;
}

/*!
	\param Observations Observed states, used with emissions 
	\param Nobs size of Observations
	\param EstimatedStates output containing the EstimatedStates at all times
	\param HMM hidden markov model used for data input
*/
int Viterbi_C(int* Observations, int Nobs, int* EstimatedStates, hmm_desc* hmm) {
	for (int i =0; i < hmm->S; i++) {
		states[0][2*i] = hmm->prior[i] * hmm->emission[i][Observations[0]];
		states[0][2*i+1] = 0;
		//printf("%f = %f * %f\n", states[0][2*i], hmm->prior[i], hmm->emission[i][Observations[0]]);
	}
	for (int i = 1; i < Nobs+1; i++) {
		ViterbiUpdate_c(states[i-1], states[i], Observations[i], hmm);
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

/*!
	ViterbitUpdate_c 
*/
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
