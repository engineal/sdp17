#include <fstream>
#include <math.h>
#include "virtual_source.h"

using namespace std;

#define samples_sec 16000

VirtualSource::VirtualSource(Channel channel, Coordinate coordinate, FilterType filter_type) : channel(channel), coord(coordinate), filter(FIRFilter(filter_type)){
	channel.addListener();
	delay = 0;
	
	buffA = new double[BUFFER_LENGTH];
	buffB = new double[BUFFER_LENGTH];
	buffC = new double[BUFFER_LENGTH];
}

VirtualSource::~VirtualSource() {
	delete [] buffA;
	delete [] buffB;
	delete [] buffC;
}

/*
 * Will eventually read from source buffer
 */
void VirtualSource::read_sample() {
	pair<double*, int> sample = channel.pop_buffer();
	sample.first;
		sample.second;
}

void VirtualSource::rotate_buffers() {
	double* tmp_buff = buffA;
	buffA = buffB;
	buffB = buffC;
	buffC = tmp_buff;
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
