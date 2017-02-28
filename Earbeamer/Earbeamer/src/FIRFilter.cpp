#include <iostream>
#include "FIRFilter.h"
#include "high_coefs.h"
#include "low_coefs.h"

using namespace std;

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

	workspace = new double[h.size() + BUFFER_LENGTH];
}

FIRFilter::~FIRFilter() {
	cout << "FIRFilter deconstructor" << endl;
	delete[] workspace;
}

//Removes all remnants of previous filtering from the workspace
void FIRFilter::flush() {
	memset(workspace, 0, (h.size() + BUFFER_LENGTH) * sizeof(double));
}

void FIRFilter::filter2(std::vector<double> samples, std::vector<double>& out) {
	int n;
	int k;

	double* p_sample;
	double* p_filter;
	double sum;

	out.resize(samples.size());

	//Place the sample at an offset into the workspace buffer
	// the offset leaves room for a filter's worth of prev data
	memcpy(&workspace[h.size() - 1], &samples[0], BUFFER_LENGTH * sizeof(double));

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

vector<double> FIRFilter::filter(vector<double> samples) {

	int n;
	int k;

	double* p_sample;
	double* p_filter;
	double sum;

	vector<double> out;
	out.resize(samples.size());

	//Place the sample at an offset into the workspace buffer
	// the offset leaves room for a filter's worth of prev data
	memcpy(&workspace[h.size() - 1], &samples[0], BUFFER_LENGTH * sizeof(double));

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
	return out;
	
}