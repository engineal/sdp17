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
	//for (int i = 0; i < BUFFER_LENGTH; i++) {
	//	output[i] = 0;
	//}

	//double temp_output[BUFFER_LENGTH];
	//for (int i = 0; i < 6; i++) {
	process_segment(output, 0);

	//for (int j = 0; j < BUFFER_LENGTH; j++) {
	//output[j] += temp_output[j];
	//}
	//}
}

/**
* Implements delay-sum on one beam
*/
void Beamformer::process_segment(double* output, int target) {
	for (int i = 0; i < BUFFER_LENGTH; i++) {
		output[i] = 0;
		for (int j = 0; j < sources.size(); j++) {
			if (i + sources[j]->delays[target] < BUFFER_LENGTH) {
				output[i] += sources[j]->buffA[i + sources[j]->delays[target]];
			}
			else {
				output[i] += sources[j]->buffB[i + sources[j]->delays[target] - BUFFER_LENGTH];
			}
		}

		output[i] /= sources.size();
	}
}

/*
 * Test code to calculate delays for each mic based on one target
 */
void Beamformer::add_target(Target target) {
	int min_delay = BUFFER_LENGTH;
	for (int i = 0; i < sources.size(); i++) {
		sources[i]->delays.push_back(sources[i]->calculate_delay_to_point(target.getPosition().x, target.getPosition().y));
		if (sources[i]->delays.back < min_delay) {
			min_delay = sources[i]->delays.back;
		}
	}
	
	cout << "target delays" << endl;
	// minimize delay on all mics
	for (int i = 0; i < sources.size(); i++) {
		sources[i]->delays.back -= min_delay;
		cout << sources[i]->delays.back << endl;
	}
	cout << endl;
}
