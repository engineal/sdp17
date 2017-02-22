#include <iostream>
#include <iomanip>
#include <mutex>
#include <ctime>
#include "beamformer.h"

using namespace std;

Beamformer::Beamformer(vector<VirtualSource> sources) : sources(sources) {
	running = false;
}

Beamformer::~Beamformer() {

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
			sources[i].read_sample();
		}

		// Start processing previously received data
		//thread t1(calculate_task, mics, outputA);
		calculate_task(output);

		// Wait for calculations to be done
		//t1.join();

		rotate_buffers();
	}

	delete output;
}

/*
* Wrapper function for rotating mic buffers
*/
void Beamformer::rotate_buffers() {
	for (int i = 0; i < sources.size(); i++) {
		sources[i].rotate_buffers();
	}
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
	process_segment(output);

	//for (int j = 0; j < BUFFER_LENGTH; j++) {
	//output[j] += temp_output[j];
	//}
	//}
}

/**
* Implements delay-sum on one beam
*/
void Beamformer::process_segment(double* output) {
	for (int i = 0; i < BUFFER_LENGTH; i++) {
		output[i] = 0;
		for (int j = 0; j < sources.size(); j++) {
			if (i + sources[j].delay < BUFFER_LENGTH) {
				output[i] += sources[j].buffA[i + sources[j].delay];
			}
			else {
				output[i] += sources[j].buffB[i + sources[j].delay - BUFFER_LENGTH];
			}
		}

		output[i] /= sources.size();
	}
}

/*
 * Test code to calculate delays for each mic based on one target
 */
void Beamformer::calculate_target_delays(Coordinate coord) {
	int min_delay = BUFFER_LENGTH;
	for (int i = 0; i < sources.size(); i++) {
		sources[i].delay = sources[i].calculate_delay_to_point(coord.x, coord.y);
		if (sources[i].delay < min_delay) {
			min_delay = sources[i].delay;
		}
	}
	
	cout << "target delays" << endl;
	// minimize delay on all mics
	for (int i = 0; i < sources.size(); i++) {
		sources[i].delay -= min_delay;
		cout << sources[i].delay << endl;
	}
	cout << endl;
}
