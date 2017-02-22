#pragma once

#include <vector>

#define BUFFER_LENGTH 1024

enum FilterType { HIGH, LOW };

class FIRFilter {
private:
	std::vector<double> h;			//filter impulse response
	double* workspace;	//holds a sample, and a filter's length worth of the previous sample
public:
	FIRFilter(FilterType type);
	~FIRFilter();
	void filter(double (&sample)[BUFFER_LENGTH], double (&buffer)[BUFFER_LENGTH]);
	void flush();
};