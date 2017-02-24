#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include "virtual_source.h"
#include "beam.h"
#include "target.h"

class Beamformer {
private:
	std::queue<std::pair<double*, int>> data_buffer;
	std::vector<VirtualSource*> sources;
	std::vector<Beam> beams;
	std::thread beamforming_thread;
	bool running;
	std::mutex data_buffer_mtx;

	void beamforming();
	void calculate_task(double* output);
	void process_segment(double* output, Beam beam);
public:
	Beamformer(std::vector<VirtualSource*> sources);
	~Beamformer();
	void start();
	void stop();
	void add_target(Target target);
	bool dataAvailable();
	std::pair<double*, int> pop_buffer();
};
