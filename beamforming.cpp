#include <iostream>
#include <thread>
#include <mutex>
#include <ctime>
#include "beamforming.h"

using namespace std;

int main(int argc, char *argv[]) {	
	iWavFile* inWavFile1 = new iWavFile("audiocheck.net_sin_1200Hz_-3dBFS_10s.wav");
	inWavFile1->displayInformation();
	
	iWavFile* inWavFile2 = new iWavFile("audiocheck.net_sin_1200Hz_-3dBFS_10s.wav");
	inWavFile2->displayInformation();
	
	oWavFile* outWavFile = new oWavFile("test.wav");
	
	// Generate linear array of equally spaced mics
	Microphone* mics[NUM_MICS];
	double x = 0;
	for (int i = 0; i < NUM_MICS; i++) {
		mics[i] = new Microphone(inWavFile1->getSampleRateHz(), x, 0.0);
		x += .171; // Mics are 7.1 cm apart, at 0 on the y axis
	}
	double* outputA = new double[BUFFER_LENGTH];
	double* outputB = new double[BUFFER_LENGTH];
	
	// calculate delay for all mics with audio source at specified x, y coordinates
	calculate_delays(mics, 0, 3);
	
	// Read in two buffers of data before processing begins
	read_inputs(mics, inWavFile1, inWavFile2);
	rotate_buffers(mics);
	read_inputs(mics, inWavFile1, inWavFile2);
	rotate_buffers(mics);

	clock_t timing[200];
	volatile int timing_index = 0;
	
	for (int i = 0; i < 50; i++) {
		// Start processing previously received data
		timing[timing_index++] = clock();
		thread t1(calculate_task, mics, outputA);
		//calculate_task(mics, outputA);
		
		// Send calculated data
		outWavFile->writeBuffer(outputB, BUFFER_LENGTH);
		
		// Read in samples from microphones
		timing[timing_index++] = clock();
		read_inputs(mics, inWavFile1, inWavFile2);
		timing[timing_index++] = clock();
		
		// Wait for calculations to be done
		t1.join();
		
		timing[timing_index++] = clock();
		
		rotate_buffers(mics);
		double* temp_output = outputA;
		outputA = outputB;
		outputB = temp_output;
	}
	
	// Print out timing results
	for (int i = 1; i < 200; i++) {
		double elapsed_secs = double(timing[i] - timing[0]) / CLOCKS_PER_SEC;
		cout << elapsed_secs << ", ";
	}
	cout << endl;
	
	delete [] outputA;
	delete [] outputB;
	
	for (int i = 0; i < NUM_MICS; i++) {
		delete mics[i];
	}
	
	outWavFile->close();
	inWavFile1->close();
	inWavFile2->close();
	
	return 0;
}

/*
 * Test code to calculate delays for each mic based on one target
 */
void calculate_delays(Microphone** mics, double x, double y) {
	int min_delay = BUFFER_LENGTH;
	for (int i = 0; i < NUM_MICS; i++) {
		mics[i]->delay = mics[i]->calculate_delay_to_point(x, y);
		if (mics[i]->delay < min_delay) {
			min_delay = mics[i]->delay;
		}
	}
	
	// minimize delay on all mics
	for (int i = 0; i < NUM_MICS; i++) {
		mics[i]->delay -= min_delay;
		cout << mics[i]->delay << endl;
	}
}

/*
 * Test code to read input from single source
 * Since reading from a single source, cannont use read_sample()
 */
void read_inputs(Microphone** mics, iWavFile* source1, iWavFile* source2) {
	if (source1->dataAvailable()) {
		double data[BUFFER_LENGTH];
		int n = source1->readBuffer(data, BUFFER_LENGTH);
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < NUM_MICS; j++) {
				if (i + mics[j]->delay < BUFFER_LENGTH) {
					mics[j]->buffC[i + mics[j]->delay] = data[i];
				} else {
					mics[j]->buffA[i + mics[j]->delay - BUFFER_LENGTH] = data[i];
				}
			}
		}
	}
}

/*
 * Wrapper function for rotating mic buffers
 */
void rotate_buffers(Microphone** mics) {
	for (int i = 0; i < NUM_MICS; i++) {
		mics[i]->rotate_buffers();
	}
}

/**
 * Thead to calculate more than one beam
 */
void calculate_task(Microphone** mics, double* output) {
	for (int i = 0; i < BUFFER_LENGTH; i++) {
		output[i] = 0;
	}
	
	double temp_output[BUFFER_LENGTH];
	for (int i = 0; i < 6; i++) {
		process_segment(mics, temp_output);
		
		for (int j = 0; j < BUFFER_LENGTH; j++) {
			output[j] += temp_output[j];
		}
	}
}

/**
 * Implements delay-sum on one beam
 */
void process_segment(Microphone** mics, double* output) {
	for (int i = 0; i < BUFFER_LENGTH; i++) {
		output[i] = 0;
		for (int j = 0; j < NUM_MICS; j++) {
			if (i + mics[j]->delay < BUFFER_LENGTH) {
				output[i] += mics[j]->buffA[i + mics[j]->delay];
			} else {
				output[i] += mics[j]->buffB[i + mics[j]->delay - BUFFER_LENGTH];
			}
		}
		output[i] /= NUM_MICS;
	}
	
	for (int i = 0; i < 5; i++) {
		cout << output[i] << "\t";
	}
	cout << endl;
}
