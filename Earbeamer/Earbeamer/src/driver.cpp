#include <iostream>
#include <windows.h>
#include <time.h>
#include "adc.h"
#include "wav-file.h"
#include "beamformer.h"

#define BUFFER_LENGTH 1024

using namespace std;

int main(int argc, char *argv[]) {
	ADC adc("");
	double low_positions[] = { -.7, -.56, -.42, -.28, -.14, 0, .14, .28, .42, .56, .7 };
	double high_positions[] = { -.42, -.35, -.28, -.21, -.14, -.07, 0, .07, .14, .21, .28 };
	vector<Microphone> mics;
	for (int i = 0; i < 11; i++) {
		Microphone mic = Microphone(adc, Coordinate(low_positions[i], 0), LOW);
		mics.push_back(mic);
	}
	for (int i = 0; i < 11; i++) {
		Microphone mic = Microphone(adc, Coordinate(high_positions[i], 0), HIGH);
		mics.push_back(mic);
	}

	Beamformer beamformer(mics);
	oWavFile outWavFile("test.wav");

	double* output = new double[BUFFER_LENGTH];
	time_t start;
	time_t current;

	time(&start);
	time(&current);
	while (difftime(current, start) < 10) {
		if (adc.dataAvailable()) {
			cout << difftime(current, start) << endl;
			adc.readBuffer(output, BUFFER_LENGTH);
			outWavFile.writeBuffer(output, BUFFER_LENGTH);
		}
		time(&current);
	}
	adc.close();
	outWavFile.close();
	getchar();
	return 0;
}
