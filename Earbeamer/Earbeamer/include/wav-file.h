#pragma once

#include <iostream>
#include <string>
#include <vector>

class WavFile {
protected:
    short int numChannels;
    unsigned int nSamplesPerSec;
    unsigned int nAvgBytesPerSec;
    short int numBlockAlingn;
    short int numBitsPerSample;
	bool open;
public:
	int delays[16];
	int getNumChannels();
	int getBitsPerSample();
	unsigned int getSampleRateHz();
	void displayInformation();
};

class iWavFile : public WavFile {
private:
	std::ifstream *fIn;
	long int numInSamples;
	long int maxInSamples;
public:
	iWavFile(std::string fileName);
	~iWavFile();
	int dataAvailable();
	std::vector<double> readBuffer(int n);
	void close();
};

class oWavFile : public WavFile {
private:
	std::ofstream *fOut;
	size_t data_chunk_pos;
	double amplify;
public:
	oWavFile(std::string fileName, double amplify);
	~oWavFile();
	void writeBuffer(std::vector<double> samples);
	void close();
};