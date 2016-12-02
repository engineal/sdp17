#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <ctime>
#include <vector>
#include "beamforming.h"

using namespace std;

int main(int argc, char *argv[]) {	
	iWavFile* inWavFile1 = new iWavFile("left.wav");
	inWavFile1->displayInformation();
	
	iWavFile* inWavFile2 = new iWavFile("right.wav");
	inWavFile2->displayInformation();
	
	oWavFile* outWavFile = new oWavFile("test.wav");
	
	// Generate linear array of equally spaced mics
	Microphone* mics[NUM_MICS];
	double x = 0;
	for (int i = 0; i < NUM_MICS; i++) {
		mics[i] = new Microphone(inWavFile1->getSampleRateHz(), x, 0.0);
		x += .171; // Mics are 17.1 cm apart, at 0 on the y axis
	}
	double* outputA = new double[BUFFER_LENGTH];
	double* outputB = new double[BUFFER_LENGTH];
	
	// calculate delay for all mics with audio source at specified x, y coordinates
	calculate_source_delays(mics, inWavFile1, 0, 3);
	calculate_source_delays(mics, inWavFile2, 3, 3);
	calculate_target_delays(mics, 3, 3);
	
	// Read in two buffers of data before processing begins
	read_inputs(mics, inWavFile1, inWavFile2);
	rotate_buffers(mics);
	read_inputs(mics, inWavFile1, inWavFile2);
	rotate_buffers(mics);

	clock_t timing[10000];
	volatile int timing_index = 0;
	
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
	
	// Print out timing results
	cout << "Filter\tBeamforming\tIO" << endl;
	for (int i = 0; i < buffers_processed; i++) {
		double elapsed_secs_filter = double(timing[(i*4)+1] - timing[i*4]) / CLOCKS_PER_SEC;
		double elapsed_secs_task = double(timing[(i*4)+2] - timing[(i*4)+1]) / CLOCKS_PER_SEC;
		double elapsed_secs_io = double(timing[(i*4)+3] - timing[(i*4)+2]) / CLOCKS_PER_SEC;
		cout << fixed << setprecision(7)
			 << elapsed_secs_filter << "\t" << elapsed_secs_task << "\t" << elapsed_secs_io << endl;
	}
	
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
 * Test code to calculate delays for each source
 */
void calculate_source_delays(Microphone** mics, WavFile* source, double x, double y) {
	int min_delay = BUFFER_LENGTH;
	for (int i = 0; i < NUM_MICS; i++) {
		source->delays[i] = mics[i]->calculate_delay_to_point(x, y);
		if (source->delays[i] < min_delay) {
			min_delay = source->delays[i];
		}
	}
	
	cout << "source delays" << endl;
	// minimize delay on all sources
	for (int i = 0; i < NUM_MICS; i++) {
		source->delays[i] -= min_delay;
		cout << source->delays[i] << endl;
	}
	cout << endl;
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

/*
 * Test code to read input from single source
 * Since reading from a single source, cannont use read_sample()
 */
void read_inputs(Microphone** mics, iWavFile* source1, iWavFile* source2) {
	if (source1->dataAvailable() && source2->dataAvailable()) {
		double data1[BUFFER_LENGTH];
		double data2[BUFFER_LENGTH];
		int n1 = source1->readBuffer(data1, BUFFER_LENGTH);
		int n2 = source2->readBuffer(data2, BUFFER_LENGTH);
		for (int i = 0; i < min(n1, n2); i++) {
			for (int j = 0; j < NUM_MICS; j++) {
				if (i + source1->delays[j] < BUFFER_LENGTH) {
					mics[j]->buffC[i + source1->delays[j]] = data1[i];
				} else {
					mics[j]->buffA[i + source1->delays[j] - BUFFER_LENGTH] = data1[i];
				}
				if (i + source2->delays[j] < BUFFER_LENGTH) {
					mics[j]->buffC[i + source2->delays[j]] += data2[i];
				} else {
					mics[j]->buffA[i + source2->delays[j] - BUFFER_LENGTH] += data2[i];
				}
			}
		}
		
		for (int i = 0; i < min(n1, n2); i++) {
			for (int j = 0; j < NUM_MICS; j++) {
				mics[j]->buffC[i] /= 2;
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
