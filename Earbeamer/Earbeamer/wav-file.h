#include <iostream>
#include <string>

#ifndef WAV_FILE_H
#define WAV_FILE_H

class WavFile {
protected:
    short int numChannels;
    unsigned int nSamplesPerSec;
    unsigned int nAvgBytesPerSec;
    short int numBlockAlingn;
    short int numBitsPerSample;

public:
	int delays[16];
	int getNumChannels();
	int getBitsPerSample();
	unsigned int getSampleRateHz();
	void displayInformation();
};

class iWavFile : public WavFile {
protected:
	std::ifstream *fIn;
	long int numInSamples;
	long int maxInSamples;

public:
	iWavFile(std::string fileName);
	~iWavFile();
	int dataAvailable();
	int readBuffer(double* samples, int n);
	void close();
};

class oWavFile : public WavFile {
protected:
	std::ofstream *fOut;
	size_t data_chunk_pos;

public:
	oWavFile(std::string fileName);
	~oWavFile();
	void writeBuffer(double* samples, int n);
	void close();
};

#endif // WAV_FILE_H