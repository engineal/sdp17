#pragma once

#include <vector>
#include "virtual_source.h"
#include "target.h"

class Beam {
private:
	std::vector<int> delays;
	int calculate_delay_between_points(Coordinate coord1, Coordinate coord2);
public:
	Beam(vector<VirtualSource*> sources);
	~Beam();
	int getDelay(int source);
	void update_delays(Target target, vector<VirtualSource*> sources);
};