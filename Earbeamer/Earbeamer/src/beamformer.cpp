#include <iostream>
#include <iomanip>
#include <mutex>
#include <ctime>
#include "beamformer.h"

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

void Beamformer::beamforming() {
	double* output = new double[1024];

	while (running) {
		// Read in samples from microphones
		for (int i = 0; i < sources.size(); i++) {
			sources[i]->read_sample();
		}

		// Start processing previously received data
		//thread t1(calculate_task, mics, outputA);
		calculate_task(output);

		// Wait for calculations to be done
		//t1.join();
	}

	delete output;
}

/**
* Thead to calculate more than one beam
*/
void Beamformer::calculate_task(double* output) {
	for (int i = 0; i < BUFFER_LENGTH; i++) {
		output[i] = 0;
	}

	double temp_output[BUFFER_LENGTH];
	for (vector<Beam>::iterator itr = beams.begin(); itr != beams.end(); ++itr) {
		process_segment(temp_output, *itr);

		for (int j = 0; j < BUFFER_LENGTH; j++) {
			output[j] += temp_output[j];
		}
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

/*
 * Test code to calculate delays for each mic based on one target
 */
void Beamformer::add_target(Target target) {
	Beam beam(sources);
	beam.update_delays(target);
	beams.push_back(beam);
}
