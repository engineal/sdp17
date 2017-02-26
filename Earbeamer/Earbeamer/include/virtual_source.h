#pragma once

#include <vector>
#include "coordinate-system.h"
#include "channel.h"
#include "FIRFilter.h"
#include "listener.h"

class VirtualSource : public IListener {
private:
	Channel* channel;
	Coordinate coord;
	FIRFilter filter;
	vector<double> buffA;
	vector<double> buffB;
public:
	VirtualSource(Channel* channel, Coordinate coordinate, FilterType filter_type);
	~VirtualSource();
	Coordinate getPosition();
	void readBuffer();
	double getSample(int index);
};