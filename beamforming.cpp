#include <iostream>
#include <thread>
#include <mutex>
#include <ctime>
#include "beamforming.h"

using namespace std;

int main(int argc, char *argv[]) {
	string inputFile = "kalia.wav";
	string outputFile = "test.wav";
	
	iWavFile* inWavFile = new iWavFile(inputFile);
	inWavFile->displayInformation();
	
	oWavFile* outWavFile = new oWavFile(outputFile);
	
	// Generate linear array of equally spaced mics
	Microphone* mics[NUM_MICS];
	double x = 0;
	for (int i = 0; i < NUM_MICS; i++) {
		mics[i] = new Microphone(x, 0.0);
		x += .071; // Mics are 7.1 cm apart, at 0 on the y axis
	}
	double* outputA = new double[BUFFER_LENGTH];
	double* outputB = new double[BUFFER_LENGTH];
	
	// calculate delay for all mics with audio source at specified x, y coordinates
	calculate_delays(mics, 2, 3);
	
	// Read in two buffers of data before processing begins
	read_inputs(mics, inWavFile);
	rotate_buffers(mics);
	read_inputs(mics, inWavFile);
	rotate_buffers(mics);
	
	/*
	for (int i = 0; i < BUFFER_LENGTH; i++) {
		cout << int(mics[0]->buffA[i]) << ", ";
	}
	cout << endl << endl;
	for (int i = 0; i < BUFFER_LENGTH; i++) {
		cout << int(mics[0]->buffB[i]) << ", ";
	}
	cout << endl << endl;
	*/

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
		read_inputs(mics, inWavFile);
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
	inWavFile->close();
	
	return 0;
}

/*
 * Test code to calculate delays for each mic based on one target
 */
void calculate_delays(Microphone** mics, double x, double y) {
	int min_delay = 0;
	for (int i = 0; i < NUM_MICS; i++) {
		mics[i]->calculate_delay(x, y);
		if (mics[i]->delay < min_delay) {
			min_delay = mics[i]->delay;
		}
	}
	
	// minimize delay on all mics
	for (int i = 0; i < NUM_MICS; i++) {
		mics[i]->delay -= min_delay;
	}
}

/*
 * Test code to read input from single source
 * Since reading from a single source, cannont use read_sample()
 */
void read_inputs(Microphone** mics, iWavFile* source) {
	if (source->dataAvailable()) {
		double data[BUFFER_LENGTH];
		int n = source->readBuffer(data, BUFFER_LENGTH);
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
}
