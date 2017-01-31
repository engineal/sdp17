#include "FIRFilter.h"

FIRFilter::FIRFilter(int order, double* coef) {

	filsize = order + 1;
	h = coef;

	workspace = (double*) malloc((filsize + BUFFER_LENGTH) * sizeof(double));
}


FIRFilter::~FIRFilter() {

	free(workspace);
}

//Removes all remnants of previous filtering from the workspace
void FIRFilter::flush() {
	memset(workspace, 0, (filsize + BUFFER_LENGTH) * sizeof(double));
}

void FIRFilter::filter(double(&sample)[BUFFER_LENGTH], double(&out)[BUFFER_LENGTH]) {

	int n;
	int k;

	double* p_sample;
	double* p_filter;

	double sum;

	//Place the sample into the workspace buffer, leaving room for a filter's worth of prev data
	memcpy(&workspace[filsize - 1], &sample, BUFFER_LENGTH * sizeof(double));

	for (n = 0; n < BUFFER_LENGTH; n++) {
		
		p_sample = &workspace[filsize - 1 + n];
		p_filter = h;
		sum = 0;
		for (k = 0; k < filsize; k++) {
			sum += (*p_sample--) * (*p_filter++);
		}
		out[n] = sum;
	}

	memmove(&workspace[0], &workspace[BUFFER_LENGTH], (filsize - 1) * sizeof(double));
}