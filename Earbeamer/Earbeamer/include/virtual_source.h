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
	
	VirtualSource(Channel* channel, Coordinate coordinate, FilterType filter_type);
	~VirtualSource();
	Coordinate getPosition();
	void read_sample();
};