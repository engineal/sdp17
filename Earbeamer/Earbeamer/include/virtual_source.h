#pragma once

#include "coordinate-system.h"
#include "channel.h"
#include "FIRFilter.h"

class VirtualSource {
private:
	Channel channel;
	Coordinate coord;
	FIRFilter filter;
public:
	double* buffA;
	double* buffB;
	double* buffC;
	int delay;
	
	VirtualSource(Channel channel, Coordinate coordinate, FilterType filter_type);
	~VirtualSource();
	void read_sample();
	void rotate_buffers();
	int calculate_delay_to_point(double x, double y);
};