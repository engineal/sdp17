#pragma once

#include <vector>
#include "coordinate-system.h"
#include "channel.h"
#include "FIRFilter.h"
#include "listener.h"

double CHEBYSHEV_WEIGHTS_20DB[11] = {1.0000, 0.8450, 1.0949, 1.3036, 1.4422, 1.4907, 1.4422, 1.3036, 1.0949, 0.8450, 1.0000 };
double UNIFORM_WEIGHTS[11] = { 1,1,1,1,1,1,1,1,1,1,1 };

class VirtualSource : public IListener {
private:
	Channel* channel;
	Coordinate coord;
	FIRFilter filter;
	double sample_weight;
	vector<double> buffA;
	vector<double> buffB;
public:
	VirtualSource(Channel* channel, Coordinate coordinate, FilterType filter_type, double weight);
	~VirtualSource();
	Coordinate getPosition();
	void readBuffer();
	double getSample(int index);
};