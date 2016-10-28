#include <iostream>
#include <ctime>
#include "beamforming.h"
#include "microphone.h"

using namespace std;

int main(int argc, char *argv[]) {
	Microphone* mics[NUM_MICS];
	for (int i = 0; i < NUM_MICS; i++) {
		mics[i] = new Microphone();
	}
	char* outputA = new char[BUFFER_LENGTH];
	char* outputB = new char[BUFFER_LENGTH];
	
	calculate_delays(mics);
	
	for (int i = 0; i < NUM_MICS; i++) {
		read_sample(mics[i]);
	}
	
	for (int i = 0; i < BUFFER_LENGTH; i++) {
		cout << int(mics[4]->buffA[i]) << ", ";
	}
	cout << endl;
	
	for (int i = 0; i < 50; i++) {
		clock_t start = clock();
		process_segment(mics, outputA);
		clock_t end = clock();
		
		double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
		cout << "Process segment time: " << elapsed_secs << endl;
	}
	
	for (int i = 0; i < BUFFER_LENGTH; i++) {
		cout << int(outputA[i]) << ", ";
	}
	cout << endl;
	
	delete outputA;
	delete outputB;
	
	for (int i = 0; i < NUM_MICS; i++) {
		delete mics[i];
	}
	
	return 0;
}

void calculate_delays(Microphone** mics) {
	char delay = 0;
	for (int i = 0; i < NUM_MICS; i++) {
		mics[i]->delay = delay;
		delay += 1;
	}
	cout << "done calculate delays" << endl;
}

/**
 * Implements delay-sum
 */
int process_segment(Microphone** mics, char* output) {
	for (int i = 0; i < BUFFER_LENGTH; i++) {
		output[i] = 0;
		for (int j = 0; j < NUM_MICS; j++) {
			if (i + mics[j]->delay < BUFFER_LENGTH) {
				output[i] += mics[j]->buffA[i + mics[j]->delay];
			} else {
				output[i] += mics[j]->buffB[NUM_MICS - i + mics[j]->delay];
			}
		}
	}
	return 0;
}

/**
 * Generate sample audio input
 */
void read_sample(Microphone* mic) {
	for (int i = 0; i < BUFFER_LENGTH; i++) {
		mic->buffA[i] = (i - mic->delay)%4;
		mic->buffB[i] = (i - mic->delay)%4;
	}
}
