#include <iostream>
#include <math.h>
#include <stdexcept>
#include "virtual_source.h"

using namespace std;

#define samples_sec 16000

VirtualSource::VirtualSource(Channel* channel, Coordinate coordinate, FilterType filter_type) : channel(channel), coord(coordinate), filter(FIRFilter(filter_type)) {
	channel->addListener(this);
	//buffB.resize(BUFFER_LENGTH);
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
	vector<double> data = channel->pop_buffer(this);
	//buffB = data;

	// apply filter
	filter.filter2(data, buffB);
	//buffB = filter.filter(data);
}

double VirtualSource::getSample(int index) {
	int buffA_size = buffA.size();
	if (index >= 0 && index < buffA_size) {
		return buffA[index];
	}
	else if ((index - buffA_size) >= 0 && (index - buffA_size) < buffB.size()) {
		return buffB[index - buffA_size];
	}
	else {
		throw out_of_range("Attempt to access memory outside of buffers");
	}
}