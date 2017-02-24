#include <iostream>
#include <math.h>
#include "virtual_source.h"

using namespace std;

#define samples_sec 16000

VirtualSource::VirtualSource(Channel* channel, Coordinate coordinate, FilterType filter_type) : channel(channel), coord(coordinate), filter(FIRFilter(filter_type)) {
	channel->addListener();
	
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

Coordinate VirtualSource::getPosition() {
	return coord;
}

/*
 * Will eventually read from source buffer
 */
void VirtualSource::read_sample() {
	// should wait for buffer to have data

	// rotate buffB into buffA
	delete [] buffA;
	buffA = buffB;
	buffA_length = buffB_length;

	pair<double*, int> sample = channel->pop_buffer();
	buffB = sample.first;
	buffB_length = sample.second;

	// apply filter
}