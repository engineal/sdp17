#include <fstream>
#include <math.h>
#include "microphone.h"
#include "beamformer.h"
#include "coordinate-system.h"

using namespace std;

Microphone::Microphone(ADC adc, Coordinate coordinate, FilterType filter_type) {
	this->adc = adc;
	coord = coordinate;
	filter = FIRFilter(filter_type);
	delay = 0;
	
	buffA = new double[BUFFER_LENGTH];
	buffB = new double[BUFFER_LENGTH];
	buffC = new double[BUFFER_LENGTH];
}

Microphone::~Microphone() {
	delete [] buffA;
	delete [] buffB;
	delete [] buffC;
}

/*
 * Will eventually read from source buffer
 */
void Microphone::read_sample() {
	for (int i = 0; i < BUFFER_LENGTH; i++) {
		buffC[i] = (i - delay)%4;
	}
}

void Microphone::rotate_buffers() {
	double* tmp_buff = buffA;
	buffA = buffB;
	buffB = buffC;
	buffC = tmp_buff;
}

/*
 * Calculate a delay for mic based on target's coordinates
 */
int Microphone::calculate_delay_to_point(double x, double y) {
	double temp = 20; // in C
	
	double distance = sqrt(pow(x - coord.x, 2) + pow(y - coord.y, 2)); // in m
	double v_sound = 331 + 0.6 * temp; // in m/s
	double t_sound = distance / v_sound; // in s
	return (int)((t_sound * samples_sec) + 0.5); // in samples
}

/*
* Test code to read input from single source
* Since reading from a single source, cannont use read_sample()
*/
void Microphone::read_input() {
	if (source1->dataAvailable()) {
		double data1[BUFFER_LENGTH];
		double data2[BUFFER_LENGTH];
		int n1 = source1->readBuffer(data1, BUFFER_LENGTH);
		int n2 = source2->readBuffer(data2, BUFFER_LENGTH);
		for (int i = 0; i < min(n1, n2); i++) {
			for (int j = 0; j < NUM_MICS; j++) {
				if (i + source1->delays[j] < BUFFER_LENGTH) {
					mics[j]->buffC[i + source1->delays[j]] = data1[i];
				}
				else {
					mics[j]->buffA[i + source1->delays[j] - BUFFER_LENGTH] = data1[i];
				}
				if (i + source2->delays[j] < BUFFER_LENGTH) {
					mics[j]->buffC[i + source2->delays[j]] += data2[i];
				}
				else {
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