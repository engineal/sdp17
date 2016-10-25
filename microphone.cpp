#include "microphone.h"
#include "beamforming.h"

Microphone::Microphone() {
	buffA = new char[SAMPLE_SIZE];
	buffB = new char[SAMPLE_SIZE];
	buffC = new char[SAMPLE_SIZE];
}

Microphone::~Microphone() {
	delete buffA;
	delete buffB;
	delete buffC;
}