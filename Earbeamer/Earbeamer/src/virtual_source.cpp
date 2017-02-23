#include <iostream>
#include <math.h>
#include "virtual_source.h"

using namespace std;

#define samples_sec 16000

VirtualSource::VirtualSource(Channel* channel, Coordinate coordinate, FilterType filter_type) : channel(channel), coord(coordinate), filter(FIRFilter(filter_type)) {
	channel->addListener();
	delay = 0;
	
	buffA = new double[BUFFER_LENGTH];
	buffA_length = BUFFER_LENGTH;
	buffB = new double[BUFFER_LENGTH];
	buffB_length = BUFFER_LENGTH;
}

VirtualSource::~VirtualSource() {
	cout << "VirtualSource deconstructor" << endl;
	delete [] buffA;
	delete [] buffB;
}

/*
 * Will eventually read from source buffer
 */
void VirtualSource::read_sample() {
	// rotate buffB into buffA
	delete [] buffA;
	buffA = buffB;
	buffA_length = buffB_length;

	pair<double*, int> sample = channel->pop_buffer();
	buffB = sample.first;
	buffB_length = sample.second;
}

/*
 * Calculate a delay for the source based on target's coordinates
 */
int VirtualSource::calculate_delay_to_point(double x, double y) {
	double temp = 20; // in C
	
	double distance = sqrt(pow(x - coord.x, 2) + pow(y - coord.y, 2)); // in m
	double v_sound = 331 + 0.6 * temp; // in m/s
	double t_sound = distance / v_sound; // in s
	return (int)((t_sound * samples_sec) + 0.5); // in samples
}
