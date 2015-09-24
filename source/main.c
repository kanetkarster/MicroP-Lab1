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
extern int ViterbiUpdate_asm(HMM *model);

int main()
{	
	HMM model;
	model.S = 3;
	model.V = 12;
	model.transition = malloc(model.S * model.S * sizeof(float));
	model.emission = malloc(model.S * model.V * sizeof(float));
	model.prior = malloc(model.S * sizeof(float));
	
	int s = ViterbiUpdate_asm(&model);
	
	printf("%d\n", s);
	return 0;
}
