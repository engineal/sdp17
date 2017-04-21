// SoftwareFiltering.cpp : Defines the entry point for the console application.
//


#include <iostream>
#include <windows.h>
#include <vector>

#include "FIRFilter.h"
#include "wav-file.h"

using namespace std;



int main1(int argc, char *argv[]) {

	iWavFile inLow("test0.wav");
	//iWavFile* inHigh = new iWavFile("audiocheck.net_sin_1800Hz_-3dBFS_3s.wav");

	oWavFile passed("passed_48_2000_4000.wav", 1);
	//oWavFile* blocked = new oWavFile("output_1800_thru_600_1200.wav");

	FIRFilter* lowFilter = new FIRFilter(HIGH);
	//FIRFilter* highFilter = new FIRFilter(HIGH);

	vector<double> inBuffer;
	vector<double> outBuffer;

	LARGE_INTEGER t0, t1, ElapsedMicroseconds, freq;
	QueryPerformanceFrequency(&freq);

	

	bool eof = false;

	while (!eof) {

		try {
			inBuffer = inLow.readBuffer(BUFFER_LENGTH);
			outBuffer = lowFilter->filter(inBuffer);
			passed.writeBuffer(outBuffer);
		}
		catch (exception e) {
			eof = true;
		}
		/*
		QueryPerformanceCounter(&t0);
		
		QueryPerformanceCounter(&t1);

		ElapsedMicroseconds.QuadPart = t1.QuadPart - t0.QuadPart;
		ElapsedMicroseconds.QuadPart *= 1000000;
		ElapsedMicroseconds.QuadPart /= freq.QuadPart;
		cout << ElapsedMicroseconds.QuadPart << endl;

		
		eof = inLow->readBuffer(inBuffer, BUFFER_LENGTH);
		*/
	}

	inLow.close();
	passed.close();


	lowFilter->flush();

	/**
	eof = inHigh->readBuffer(inBuffer, BUFFER_LENGTH);

	while (eof != -1) {

		lowFilter->filter(inBuffer, outBuffer);
		blocked->writeBuffer(outBuffer, BUFFER_LENGTH);
		eof = inHigh->readBuffer(inBuffer, BUFFER_LENGTH);
	}
	**/

}