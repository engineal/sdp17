#pragma once

#include <string>
#include <vector>
#include <thread>
#include "virtual_source.h"

class Beamformer {
private:
	std::vector<VirtualSource> sources;
	thread beamforming_thread;
	bool running;

	void beamforming();
	void rotate_buffers();
	void calculate_task(double* output);
	void process_segment(double* output);
	void calculate_target_delays(Coordinate coord);
public:
	Beamformer(std::vector<VirtualSource> sources);
	~Beamformer();
	void start();
	void stop();
};
