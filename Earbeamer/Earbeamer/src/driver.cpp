#include <iostream>
#include <windows.h>
#include "adc.h"
#include "wav-file.h"
#include <time.h>

#define BUFFER_LENGTH 1024

using namespace std;

int main(int argc, char *argv[]) {
	ADC adc = ADC("");
	oWavFile outWavFile = oWavFile("test.wav");

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

