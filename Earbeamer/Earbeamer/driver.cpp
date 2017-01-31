

#include "FIRFilter.h"
#include "wav-file.h"
#include "high_coefs.h"
#include "low_coefs.h"

using namespace std;



int main(int argc, char *argv[]) {

	iWavFile* inLow = new iWavFile("audiocheck.net_sin_800Hz_-3dBFS_10s.wav");
	iWavFile* inHigh = new iWavFile("audiocheck.net_sin_1800Hz_-3dBFS_3s.wav");

	oWavFile* passed = new oWavFile("output_800_thru_600_1200.wav");
	oWavFile* blocked = new oWavFile("output_1800_thru_600_1200.wav");

	FIRFilter* lowFilter = new FIRFilter(114, LOW_COEFS);
	FIRFilter* highFilter = new FIRFilter(76, HIGH_COEFS);

	double inBuffer[BUFFER_LENGTH];
	double outBuffer[BUFFER_LENGTH];

	int eof = inLow->readBuffer(inBuffer, BUFFER_LENGTH);

	while (eof != -1) {

		lowFilter->filter(inBuffer, outBuffer);
		passed->writeBuffer(outBuffer, BUFFER_LENGTH);
		eof = inLow->readBuffer(inBuffer, BUFFER_LENGTH);

	}

	lowFilter->flush();
	eof = inHigh->readBuffer(inBuffer, BUFFER_LENGTH);

	while (eof != -1) {
		
		lowFilter->filter(inBuffer, outBuffer);
		blocked->writeBuffer(outBuffer, BUFFER_LENGTH);
		eof = inHigh->readBuffer(inBuffer, BUFFER_LENGTH);
	}




	return 1;
}


void readFile(iWavFile* source) {

	




}