#include <iostream>
#include <windows.h>
#include <time.h>
#include "channel.h"
#include "virtual_source.h"
#include "adc.h"
#include "beamformer.h"
#include "wav-file.h"
#include "room.h"

using namespace std;

vector<VirtualSource*> createSources(vector<Channel*> channels) {
	vector<pair<double, Channel*>> low_positions = {
		pair<double, Channel*>(-.70, channels[0]),
		pair<double, Channel*>(-.56, channels[1]),
		pair<double, Channel*>(-.42, channels[2]),
		pair<double, Channel*>(-.28, channels[4]),
		pair<double, Channel*>(-.14, channels[6]),
		pair<double, Channel*>(0, channels[8]),
		pair<double, Channel*>(.14, channels[10]),
		pair<double, Channel*>(.28, channels[12]),
		pair<double, Channel*>(.42, channels[13]),
		pair<double, Channel*>(.56, channels[14]),
		pair<double, Channel*>(.70, channels[15])
	};
	vector<pair<double, Channel*>> high_positions = {
		pair<double, Channel*>(-.42, channels[2]),
		pair<double, Channel*>(-.35, channels[3]),
		pair<double, Channel*>(-.28, channels[4]),
		pair<double, Channel*>(-.21, channels[5]),
		pair<double, Channel*>(-.14, channels[6]),
		pair<double, Channel*>(-.07, channels[7]),
		pair<double, Channel*>(0, channels[8]),
		pair<double, Channel*>(.07, channels[9]),
		pair<double, Channel*>(.14, channels[10]),
		pair<double, Channel*>(.21, channels[11]),
		pair<double, Channel*>(.28, channels[12])
	};

	vector<VirtualSource*> sources;
	for (vector<pair<double, Channel*>>::iterator itr = low_positions.begin(); itr != low_positions.end(); ++itr) {
		sources.push_back(new VirtualSource(itr->second, Coordinate(itr->first, 0), LOW));
	}
	for (vector<pair<double, Channel*>>::iterator itr = high_positions.begin(); itr != high_positions.end(); ++itr) {
		sources.push_back(new VirtualSource(itr->second, Coordinate(itr->first, 0), HIGH));
	}

	return sources;
}

int main(int argc, char *argv[]) {
	vector<Channel*> channels;
	for (int i = 0; i < 16; i++) {
		channels.push_back(new Channel("Dev1/ai" + to_string(i)));
	}
	vector<VirtualSource*> sources = createSources(channels);

	try {
		ADC adc(channels, 15625.0);
		//Room room(CoordinateSystem());

		Beamformer beamformer(sources);
		oWavFile outWavFile("test.wav");

		time_t start;
		time_t current;

		adc.start();
		beamformer.start();

		time(&start);
		time(&current);
		while (difftime(current, start) < 60) {
			beamformer.waitForData();
			vector<double> output = beamformer.pop_buffer();
			outWavFile.writeBuffer(&output[0], (int)output.size());

			//cout << difftime(current, start) << endl;
			time(&current);
		}

		beamformer.stop();
		adc.stop();

		outWavFile.close();
	}
	catch (exception& e) {
		cout << e.what() << endl;
	}

	//getchar();
	return 0;
}
