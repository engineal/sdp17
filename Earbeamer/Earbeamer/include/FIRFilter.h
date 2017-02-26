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
	void filter(std::vector<double> samples, std::vector<double> out);
	void flush();
};