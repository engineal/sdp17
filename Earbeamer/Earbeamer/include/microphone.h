#pragma once

#include "coordinate-system.h"
#include "adc.h"
#include "FIRFilter.h"

class Microphone {
protected:
	ADC adc;
	Coordinate coord;
	FIRFilter filter;

public:
	double* buffA;
	double* buffB;
	double* buffC;
	int delay;
	
	Microphone(ADC adc, Coordinate coordinate, FilterType filter);
	~Microphone();
	void read_sample();
	void rotate_buffers();
	int calculate_delay_to_point(double x, double y);
};