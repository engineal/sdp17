#include <fstream>
#include <iostream>
#include <math.h>
#include "wav-file.h"

using namespace std;

namespace little_endian_io
{
	template <typename Word>
	std::istream& read_word(std::istream& ins, Word& value, unsigned size = sizeof(Word)) {
		unsigned bytes = size;
		value = 0;
		char tmp;
		for (; size; --size) {
			ins.get(tmp);
			value |= (tmp & 0xFF) << ((bytes - size) * 8);
		}
		return ins;
	}
	
	template <typename Word>
	std::ostream& write_word(std::ostream& outs, Word value, unsigned size = sizeof(Word)) {
		for (; size; --size) {
			outs.put(static_cast <char> (value & 0xFF));
			value >>= 8;
		}
		return outs;
	}
}
using namespace little_endian_io;

/*-----------------------------------------------------------------------------
 * WavFile
 *-----------------------------------------------------------------------------*/

int WavFile::getNumChannels() {
    return numChannels;
}

int WavFile::getBitsPerSample() {
    return numBitsPerSample;
}

unsigned int WavFile::getSampleRateHz() {
    return nSamplesPerSec;
}

void WavFile::displayInformation() {
    cout << "-----------------------------------------------------" << endl;
    cout << "Number of channels: " << numChannels << endl;
    cout << "Sample rate: " << nSamplesPerSec << " (Hz)" << endl;
	cout << "Byte Rate: " << nAvgBytesPerSec << endl;
	cout << "Block Align: " << numBlockAlingn << endl;
    cout << "Bits per sample: " << numBitsPerSample << endl;
    cout << "-----------------------------------------------------" << endl;
}

/*-----------------------------------------------------------------------------
 * iWavFile
 *-----------------------------------------------------------------------------*/

iWavFile::iWavFile(string fileName) {
	fIn = new ifstream(fileName, ios::binary);
	open = true;
	if(!fIn) {
		cerr << "Cannot open " << fileName << endl;
		exit(-1);
	}
	
	string str(4, ' ');
	fIn->read(&str[0], 4);
	if (str != "RIFF") {
		cerr << "File is not in RIFF format!" << str << endl;
		exit(-1);
	}
	
	fIn->seekg(4, ios::cur); // skip RIFF chunk size
	
	fIn->read(&str[0], 4);
	if (str != "WAVE") {
		cerr << "File is not in WAVE format!" << str << endl;
		exit(-1);
	}
	
	fIn->read(&str[0], 4);
	if (str != "fmt ") {
		cerr << "File does not have fmt sub-chunk!" << str << endl;
		exit(-1);
	}
	
	size_t pcmHeaderLength;
	read_word(*fIn, pcmHeaderLength, 4);
	
	short int wFormatTag;
	read_word(*fIn, wFormatTag, 2);
	if(wFormatTag != 1) {
        cerr << "Compression not supported!" << wFormatTag << endl;
        exit(-1);
    }
	
	read_word(*fIn, numChannels, 2);
	if(numChannels > 2) {
        cerr << "More than 2 channels is not supported!" << endl;
        exit(-1);
    }
	
	read_word(*fIn, nSamplesPerSec, 4);
	read_word(*fIn, nAvgBytesPerSec, 4);
	read_word(*fIn, numBlockAlingn, 2);
	read_word(*fIn, numBitsPerSample, 2);
    if((numBitsPerSample != 16) && (numBitsPerSample != 8)) {
        cerr << "Cannot support " << numBitsPerSample << " bits per sample!" << endl;
        exit(-1);
    }
	
	fIn->seekg(pcmHeaderLength - 16, ios::cur);
	
	// Skip extra chunks
	int attempts = 0;
    while (true) {
        if (attempts > 10) {
            cerr << "Too many chunks" << endl;
            exit(-1);
        }

        fIn->read(&str[0], 4);
		if (str == "data") {
			break;
		}
		
        attempts++;
		
		size_t dLen;
		read_word(*fIn, dLen, 4);
		fIn->seekg(dLen, ios::cur);
    }
	
    numInSamples = 0;
	
	read_word(*fIn, maxInSamples, 4);
    maxInSamples /= (numBitsPerSample / 8) * numChannels;
}

iWavFile::~iWavFile() {
	if (open) {
		close();
	}
}

int iWavFile::dataAvailable() {
    if (numInSamples >= maxInSamples) {
        return 0;
    }
    return 1;
}

int iWavFile::readBuffer(double* samples, int n) {
	if (maxInSamples > 0 && numInSamples < maxInSamples) {
		short int num_bytes = 1;
		if (numBitsPerSample == 16) {
			num_bytes = 2;
		}
		
		int i;
		short int value;
		for (i = 0; i < n; i++) {
			if (numInSamples >= maxInSamples) {
				break;
			}
			if (numChannels == 2) {
				read_word(*fIn, value, num_bytes);
				read_word(*fIn, value, num_bytes);
				value /= 2;
			} else {
				read_word(*fIn, value, num_bytes);
			}
			
			samples[i] = (double)value;
			numInSamples++;
		}
		return i;
	} else {
		cout << "There is no data to be read!" << endl;
        return -1;
	}
}

void iWavFile::close() {
	fIn->close();
	delete fIn;
	open = false;
}

/*-----------------------------------------------------------------------------
 * oWavFile
 *-----------------------------------------------------------------------------*/


oWavFile::oWavFile(string fileName) {
	fOut = new ofstream(fileName, ios::binary);
	open = true;
	
	numChannels = 1;
    nSamplesPerSec = 15625;
    nAvgBytesPerSec = 31250;
    numBlockAlingn = 2;
    numBitsPerSample = 16;

	// Write the file headers
	*fOut << "RIFF----WAVEfmt "; // (chunk size to be filled in later)
	write_word(*fOut, 16, 4); // no extension data
	write_word(*fOut, 1, 2); // PCM - integer samples
	write_word(*fOut, numChannels, 2); // one channel (mono file)
	write_word(*fOut, nSamplesPerSec, 4); // samples per second (Hz)
	write_word(*fOut, nAvgBytesPerSec, 4); // (Sample Rate * BitsPerSample * Channels) / 8
	write_word(*fOut, numBlockAlingn, 2); // data block size (size of two integer samples, one for each channel, in bytes)
	write_word(*fOut, numBitsPerSample, 2); // number of bits per sample (use a multiple of 8)

	// Write the data chunk header
	data_chunk_pos = fOut->tellp();
	*fOut << "data----";  // (chunk size to be filled in later)
}

oWavFile::~oWavFile() {
	if (open) {
		close();
	}
}

void oWavFile::writeBuffer(double* samples, int n) {
	short int num_bytes = 1;
	if (numBitsPerSample == 16) {
		num_bytes = 2;
	}
	
	for (int i = 0; i < n; i++) {
		short sample = (short)(samples[i] * 32000);
		if (numChannels == 2) {
			write_word(*fOut, sample, num_bytes);
			write_word(*fOut, sample, num_bytes);
		} else {
			write_word(*fOut, sample, num_bytes);
		}
	}
}

void oWavFile::close() {
	// (We'll need the final file size to fix the chunk sizes above)
	size_t file_length = fOut->tellp();

	// Fix the data chunk header to contain the data size
	fOut->seekp(data_chunk_pos + 4);
	write_word(*fOut, file_length - (data_chunk_pos + 8), 4);

	// Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
	fOut->seekp(0 + 4);
	write_word(*fOut, file_length - 8, 4);
	
	fOut->close();
	delete fOut;
	open = false;
}
