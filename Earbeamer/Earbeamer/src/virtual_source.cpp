#include <iostream>
#include <math.h>
#include <stdexcept>
#include "virtual_source.h"

using namespace std;

#define samples_sec 16000

VirtualSource::VirtualSource(Channel* channel, Coordinate coordinate, FilterType filter_type) : channel(channel), coord(coordinate), filter(FIRFilter(filter_type)) {
	channel->addListener();
}

VirtualSource::~VirtualSource() {
	cout << "VirtualSource deconstructor" << endl;
}

Coordinate VirtualSource::getPosition() {
	return coord;
}

/*
 * Will eventually read from source buffer
 */
void VirtualSource::readBuffer() {
	// should wait for buffer to have data
	channel->waitForData();

	// rotate buffB into buffA
	buffA = buffB;

	// channel should have data
	vector<double> data = channel->pop_buffer();
	buffB = data;

	// apply filter
}

double VirtualSource::getSample(int index) {
	if (index >= 0 && index < buffA.size()) {
		return buffA[index];
	}
	else if ((index - buffA.size()) >= 0 && (index - buffA.size()) < buffB.size()) {
		return buffB[(index - buffA.size())];
	}
	else {
		throw out_of_range("Attempt to access memory outside of buffers");
	}
}