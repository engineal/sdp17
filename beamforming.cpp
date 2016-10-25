#include <iostream>
#include "beamforming.h"
#include "microphone.h"

using namespace std;

int main(int argc, char *argv[]){
	Microphone* mics[NUM_MICS];
	for (int i = 0; i < NUM_MICS; i++) {
		mics[i] = new Microphone();
	}
	
	char delays[NUM_MICS];
	calculate_delays(delays);
	
	for (int i = 0; i < NUM_MICS; i++) {
		delete mics[i];
	}
	
	return 0;
}

void calculate_delays(char* delays) {
	char delay = 0;
	for (int i = 0; i < NUM_MICS; i++) {
		delays[i] = delay;
		delay += .001;
	}
	cout << "done calculate delays" << endl;
}

/**
 * Implements delay-sum
 */
int process_segment() {
	return 0;
}

/**
 * Generate sample audio input
 */
int read_sample() {
	return 0;
}