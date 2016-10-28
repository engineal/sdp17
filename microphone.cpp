#include "microphone.h"
#include "beamforming.h"

Microphone::Microphone() {
	buffA = new char[BUFFER_LENGTH];
	buffB = new char[BUFFER_LENGTH];
	buffC = new char[BUFFER_LENGTH];
	delay = 0;
}

Microphone::~Microphone() {
	delete buffA;
	delete buffB;
	delete buffC;
}