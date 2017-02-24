#pragma once

#include <string>
#include <vector>
#include <thread>
#include "virtual_source.h"
#include "target.h"

class Beamformer {
private:
	std::vector<VirtualSource*> sources;
	thread beamforming_thread;
	bool running;

	void beamforming();
	void calculate_task(double* output);
	void process_segment(double* output, int target);
	void add_target(Target target);
public:
	Beamformer(std::vector<VirtualSource*> sources);
	~Beamformer();
	void start();
	void stop();
};
