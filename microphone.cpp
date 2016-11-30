#include <fstream>
#include <math.h>
#include "microphone.h"
#include "beamforming.h"

using namespace std;

Microphone::Microphone(double x, double y) {
	x_pos = x;
	y_pos = y;
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
void Microphone::calculate_delay(double x, double y) {
	double samples_sec = 16000;
	double temp = 20; // in C
	
	double distance = sqrt(pow(x - x_pos, 2) + pow(y - y_pos, 2)); // in m
	double v_sound = 331 + 0.6 * temp; // in m/s
	double t_sound = distance / v_sound; // in s
	delay = (int)((t_sound * samples_sec) + 0.5); // in samples
}