#include <iostream>
#include <windows.h>
#include <time.h>
#include "channel.h"
#include "virtual_source.h"
#include "adc.h"
#include "beamformer.h"
#include "wav-file.h"

using namespace std;

vector<VirtualSource> createSources(vector<Channel> channels) {
	vector<pair<double, Channel>> low_positions = {
		pair<double, Channel>(-.7, channels[0]),
		pair<double, Channel>(-.56, channels[1]),
		pair<double, Channel>(-.42, channels[2]),
		pair<double, Channel>(-.28, channels[3]),
		pair<double, Channel>(-.14, channels[4]),
		pair<double, Channel>(0, channels[5]),
		pair<double, Channel>(.14, channels[6]),
		pair<double, Channel>(.28, channels[7]),
		pair<double, Channel>(.42, channels[8]),
		pair<double, Channel>(.56, channels[9]),
		pair<double, Channel>(.7, channels[10])
	};
	vector<pair<double, Channel>> high_positions = {
		pair<double, Channel>(-.42, channels[0]),
		pair<double, Channel>(-.35, channels[1]),
		pair<double, Channel>(-.28, channels[2]),
		pair<double, Channel>(-.21, channels[3]),
		pair<double, Channel>(-.14, channels[4]),
		pair<double, Channel>(-.07, channels[5]),
		pair<double, Channel>(0, channels[6]),
		pair<double, Channel>(.07, channels[7]),
		pair<double, Channel>(.14, channels[8]),
		pair<double, Channel>(.21, channels[9]),
		pair<double, Channel>(.28, channels[10])
	};

	vector<VirtualSource> sources;
	for (vector<pair<double, Channel>>::iterator itr = low_positions.begin(); itr != low_positions.end(); ++itr) {
		VirtualSource source = VirtualSource(itr->second, Coordinate(itr->first, 0), LOW);
		sources.push_back(source);
	}
	for (vector<pair<double, Channel>>::iterator itr = high_positions.begin(); itr != high_positions.end(); ++itr) {
		VirtualSource source = VirtualSource(itr->second, Coordinate(itr->first, 0), HIGH);
		sources.push_back(source);
	}

	return sources;
}

int main(int argc, char *argv[]) {
	vector<Channel> channels;
	for (int i = 0; i < 16; i++) {
		channels.push_back(Channel("Dev1/ai" + i));
	}
	ADC adc(channels, 16000.0);
	vector<VirtualSource> sources = createSources(channels);
	Beamformer beamformer(sources);
	oWavFile outWavFile("test.wav");

	double* output = new double[BUFFER_LENGTH];
	time_t start;
	time_t current;

	adc.start();
	beamformer.start();

	time(&start);
	time(&current);
	while (difftime(current, start) < 10) {
		//if (adc.dataAvailable()) {
			cout << difftime(current, start) << endl;
			//adc.readBuffer(output, BUFFER_LENGTH);
			outWavFile.writeBuffer(output, BUFFER_LENGTH);
		//}
		time(&current);
	}

	beamformer.stop();
	adc.stop();

	outWavFile.close();
	getchar();
	return 0;
}
