#pragma once

#include <string>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "virtual_source.h"
#include "beam.h"
#include "target.h"

class Beamformer {
private:
	std::queue<std::vector<double>> data_buffer;
	std::vector<VirtualSource*> sources;
	std::map<Target*, Beam*> beams;
	std::thread beamforming_thread;
	bool running;
	std::mutex data_buffer_mtx;
	std::condition_variable data_buffer_cv;
	std::mutex beams_mtx;

	void beamforming();
	std::vector<double> calculate_task();
	void process_beam(Beam& beam, vector<double>& output);
public:
	Beamformer(std::vector<VirtualSource*> sources);
	~Beamformer();
	void start();
	void stop();
	void updateTargets(std::map<UINT64, Target*> targets);
	bool dataAvailable();
	void waitForData();
	std::vector<double> pop_buffer();
};
