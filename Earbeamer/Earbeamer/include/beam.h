#pragma once

#include <vector>
#include "virtual_source.h"
#include "target.h"


class Beam {
private:
	std::vector<int> delays;
	int calculate_delay_between_points(Coordinate coord1, Coordinate coord2);
	bool muted;
	double volume_scalar;
public:
	bool isMuted();
	void setMuted(bool mute);
	double getVolumeScalar();
	void updateAvgVolume(double avg_volume);
	Beam(vector<VirtualSource*> sources);
	~Beam();
	int getDelay(int source);
	void update_plane_delays(Target target, vector<VirtualSource*> sources);
	void update_sphere_delays(Target target, vector<VirtualSource*> sources);
};