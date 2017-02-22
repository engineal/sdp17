#include "FIRFilter.h"
#include "high_coefs.h"
#include "low_coefs.h"

FIRFilter::FIRFilter(FilterType type) {
	switch (type)
	{
	case HIGH:
		h = HIGH_COEFS;
		break;
	case LOW:
		h = LOW_COEFS;
		break;
	default:
		break;
	}

	workspace = (double*)malloc((h.size() + BUFFER_LENGTH) * sizeof(double));
}

FIRFilter::~FIRFilter() {
	free(workspace);
}

//Removes all remnants of previous filtering from the workspace
void FIRFilter::flush() {
	memset(workspace, 0, (h.size() + BUFFER_LENGTH) * sizeof(double));
}

void FIRFilter::filter(double(&sample)[BUFFER_LENGTH], double(&out)[BUFFER_LENGTH]) {

	int n;
	int k;

	double* p_sample;
	double* p_filter;
	double sum;

	//Place the sample at an offset into the workspace buffer
	// the offset leaves room for a filter's worth of prev data
	memcpy(&workspace[h.size() - 1], &sample, BUFFER_LENGTH * sizeof(double));

	for (n = 0; n < BUFFER_LENGTH; n++) {
		
		p_sample = &workspace[h.size() - 1 + n];
		p_filter = &h[0];
		sum = 0;
		for (k = 0; k < h.size(); k++) {
			sum += (*p_sample--) * (*p_filter++);
		}
		out[n] = sum;
	}

	memmove(&workspace[0], &workspace[BUFFER_LENGTH], (h.size() - 1) * sizeof(double));
	
}