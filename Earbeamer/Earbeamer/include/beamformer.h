#pragma once

#include <string>
#include<vector>
#include "microphone.h"
#include "wav-file.h"

#define BUFFER_LENGTH 1024

class Beamformer {
public:
	Beamformer(vector<Microphone> mics);
	~Beamformer();
	void start();
	void stop();

private:
	void calculate_target_delays(double x, double y);
	void rotate_buffers();
	void calculate_task(double* output);
	void process_segment(double* output);
	void beamforming();
	vector<Microphone> mics;
	thread beamforming_thread;
	bool running;
};
