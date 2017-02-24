#include <iostream>
#include <iomanip>
#include <mutex>
#include <ctime>
#include "beamformer.h"
#include "cexception.h"

using namespace std;

Beamformer::Beamformer(vector<VirtualSource*> sources) : sources(sources) {
	running = false;
}

Beamformer::~Beamformer() {
	cout << "Beamformer deconstructor" << endl;
}

void Beamformer::start() {
	running = true;
	//spawn processing thread
	beamforming_thread = thread(&Beamformer::beamforming, this);
}

void Beamformer::stop() {
	running = false;
	//spawn processing thread
	beamforming_thread.join();
}

/*
* Create a beam for the target
*/
void Beamformer::add_target(Target target) {
	Beam beam(sources);
	beam.update_delays(target);
	beams.push_back(beam);
}

bool Beamformer::dataAvailable() {
	data_buffer_mtx.lock();
	return data_buffer.size() > 0;
	data_buffer_mtx.unlock();
}

pair<double*, int> Beamformer::pop_buffer() {
	pair<double*, int> tmp;

	data_buffer_mtx.lock();
	if (data_buffer.size() > 0) {
		tmp = data_buffer.front();
		data_buffer.pop();
		data_buffer_mtx.unlock();
		return tmp;
	}
	data_buffer_mtx.unlock();
	throw ElementNotAvailableException();
}

void Beamformer::beamforming() {
	while (running) {
		// Read in samples from microphones
		for (int i = 0; i < sources.size(); i++) {
			sources[i]->read_sample();
		}

		double* output = new double[BUFFER_LENGTH];
		calculate_task(output);

		// push output into buffer
		data_buffer_mtx.lock();
		data_buffer.push(pair<double*, int>(output, BUFFER_LENGTH));
		data_buffer_mtx.unlock();
	}
}

/**
* Thead to calculate more than one beam
*/
void Beamformer::calculate_task(double* output) {
	// Clear output buffer first
	for (int i = 0; i < BUFFER_LENGTH; i++) {
		output[i] = 0;
	}

	// Calculate each beam separately
	double temp_output[BUFFER_LENGTH];
	for (vector<Beam>::iterator itr = beams.begin(); itr != beams.end(); ++itr) {
		process_segment(temp_output, *itr);

		for (int j = 0; j < BUFFER_LENGTH; j++) {
			output[j] += temp_output[j];
		}
	}

	// Normalize the audio level so no clipping happens
	for (int i = 0; i < BUFFER_LENGTH; i++) {
		output[i] /= beams.size();
	}
}

/**
* Implements delay-sum on one beam
*/
void Beamformer::process_segment(double* output, Beam beam) {
	for (int i = 0; i < BUFFER_LENGTH; i++) {
		output[i] = 0;
		for (int j = 0; j < sources.size(); j++) {
			if (i + beam.getDelay(sources[j]) < BUFFER_LENGTH) {
				output[i] += sources[j]->buffA[i + beam.getDelay(sources[j])];
			}
			else {
				output[i] += sources[j]->buffB[i + beam.getDelay(sources[j]) - BUFFER_LENGTH];
			}
		}

		output[i] /= sources.size();
	}
}