#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <ctime>
#include <vector>
#include "beamformer.h"

using namespace std;

Beamformer::Beamformer(vector<Microphone> mics) : mics(mics) {
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
	while (running) {
		// Start processing previously received data
		//thread t1(calculate_task, mics, outputA);
		calculate_task(mics, outputA);

		// Send calculated data
		outWavFile->writeBuffer(outputB, BUFFER_LENGTH);

		// Read in samples from microphones
		read_inputs(mics, inWavFile1, inWavFile2);

		// Wait for calculations to be done
		//t1.join();

		buffers_processed++;

		rotate_buffers(mics);
		double* temp_output = outputA;
		outputA = outputB;
		outputB = temp_output;
	}
}

/*
* Wrapper function for rotating mic buffers
*/
void Beamformer::rotate_buffers() {
	for (int i = 0; i < mics.size(); i++) {
		mics[i].rotate_buffers();
	}
}

int main() {
	// Generate linear array of equally spaced mics
	double* outputA = new double[BUFFER_LENGTH];
	double* outputB = new double[BUFFER_LENGTH];
	
	// calculate delay for all mics with audio source at specified x, y coordinates
	calculate_target_delays(mics, 3, 3);
	
	// Read in two buffers of data before processing begins
	read_inputs(mics, inWavFile1, inWavFile2);
	rotate_buffers(mics);
	read_inputs(mics, inWavFile1, inWavFile2);
	rotate_buffers(mics);

	int buffers_processed = 0;
	while (inWavFile1->dataAvailable() && inWavFile2->dataAvailable()) {
		timing[timing_index++] = clock();
		// Timing for filter
		double d1 = rand();
		double d2 = rand();
		double d3 = rand();
		double d4 = rand();
		double d5 = rand();
		double temp[BUFFER_LENGTH];
		for (int j = 0; j < NUM_MICS; j++) {
			for (int i = 0; i < BUFFER_LENGTH; i++) {
				temp[i] = d1 * (j * d2 + d3 * (i + d2 + d3) + d4) + d5;
			}
		}
		
		timing[timing_index++] = clock();
		// Start processing previously received data
		//thread t1(calculate_task, mics, outputA);
		calculate_task(mics, outputA);
		timing[timing_index++] = clock();
		
		// Send calculated data
		outWavFile->writeBuffer(outputB, BUFFER_LENGTH);
		
		// Read in samples from microphones
		read_inputs(mics, inWavFile1, inWavFile2);
		timing[timing_index++] = clock();
		
		// Wait for calculations to be done
		//t1.join();
		
		buffers_processed++;
		
		rotate_buffers(mics);
		double* temp_output = outputA;
		outputA = outputB;
		outputB = temp_output;
	}
	
	delete [] outputA;
	delete [] outputB;
	
	return 0;
}

/*
 * Test code to calculate delays for each mic based on one target
 */
void calculate_target_delays(Microphone** mics, double x, double y) {
	int min_delay = BUFFER_LENGTH;
	for (int i = 0; i < NUM_MICS; i++) {
		mics[i]->delay = mics[i]->calculate_delay_to_point(x, y);
		if (mics[i]->delay < min_delay) {
			min_delay = mics[i]->delay;
		}
	}
	
	cout << "target delays" << endl;
	// minimize delay on all mics
	for (int i = 0; i < NUM_MICS; i++) {
		mics[i]->delay -= min_delay;
		cout << mics[i]->delay << endl;
	}
	cout << endl;
}

/**
 * Thead to calculate more than one beam
 */
void calculate_task(Microphone** mics, double* output) {
	//for (int i = 0; i < BUFFER_LENGTH; i++) {
	//	output[i] = 0;
	//}
	
	//double temp_output[BUFFER_LENGTH];
	//for (int i = 0; i < 6; i++) {
		process_segment(mics, output);
		
		//for (int j = 0; j < BUFFER_LENGTH; j++) {
			//output[j] += temp_output[j];
		//}
	//}
}

/**
 * Implements delay-sum on one beam
 */
void process_segment(Microphone** mics, double* output) {
	for (int i = 0; i < BUFFER_LENGTH; i++) {
		output[i] = 0;
		for (int j = 0; j < mics.size(); j++) {
			if (i + mics[j]->delay < BUFFER_LENGTH) {
				output[i] += mics[j]->buffA[i + mics[j]->delay];
			} else {
				output[i] += mics[j]->buffB[i + mics[j]->delay - BUFFER_LENGTH];
			}
		}

		output[i] /= mics.size();
	}
}
