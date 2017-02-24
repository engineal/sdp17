#pragma once

#include "coordinate-system.h"
#include "channel.h"
#include "FIRFilter.h"

class VirtualSource {
private:
	Channel* channel;
	Coordinate coord;
	FIRFilter filter;
public:
	double* buffA;
	int buffA_length;
	double* buffB;
	int buffB_length;
	std::vector<int> delays;
	
	VirtualSource(Channel* channel, Coordinate coordinate, FilterType filter_type);
	~VirtualSource();
	void read_sample();
	int calculate_delay_to_point(double x, double y);
};