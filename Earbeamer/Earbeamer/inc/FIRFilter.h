#pragma once

#ifndef FIR_FILTER_H_
#define FIR_FILTER_H_

#include "beamforming.h"



class FIRFilter {

private:
	int filsize;		//number of coefficients
	double* h;			//filter impulse response
	double* workspace;	//holds a sample, and a filter's length worth of the previous sample

public:
	FIRFilter(int ord, double* coef);
	~FIRFilter();
	void filter(double (&sample)[BUFFER_LENGTH], double (&buffer)[BUFFER_LENGTH]);
	void flush();

};

#endif