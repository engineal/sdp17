#include <iostream>
#include "websocket_server.h"// <--- Must be included before windows.h
#include <windows.h>
#include <time.h>
#include "channel.h"
#include "virtual_source.h"

#include "adc.h"
#include "beamformer.h"
#include "wav-file.h"
#include "room.h"
#include <map>



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

	CameraSpacePoint left = CameraSpacePoint();
	left.X = 0.7;
	left.Y = 0;
	left.Z = .42;

	CameraSpacePoint right = CameraSpacePoint();
	right.X = -0.7;
	right.Y = 0;
	right.Z = 0.42;

	CoordinateSystem grid = CoordinateSystem(left, right);


	try {
		ADC adc(channels, 15625.0);
		Room* room = new Room(grid);

		room->Init();
		

		Beamformer beamformer(sources);
		room->beginMonitoring(&beamformer);

		oWavFile outWavFile("test.wav");
		oWavFile* outputFiles[16];
		IListener listeners[16];
		for (int i = 0; i < channels.size(); i++) {
			outputFiles[i] = new oWavFile("test" + to_string(i) + ".wav");
			channels[i]->addListener(&listeners[i]);
		}

		time_t start;
		time_t current;

		adc.start();
		beamformer.start();
		
		time(&start);
		time(&current);
		
		while (difftime(current, start) < 30) {
			for (int i = 0; i < 16; i++) {
				channels[i]->waitForData();
				vector<double> data = channels[i]->pop_buffer(&listeners[i]);
				outputFiles[i]->writeBuffer(&data[0], data.size());
			}

			beamformer.waitForData();
			vector<double> output = beamformer.pop_buffer();
			outWavFile.writeBuffer(&output[0], (int)output.size());
			
			//cout << difftime(current, start) << endl;
			time(&current);
		}

		for (int i = 0; i < channels.size(); i++) {
			outputFiles[i]->close();
			channels[i]->removeListener(&listeners[i]);
		}

		room->stop();
		beamformer.stop();
		adc.stop();

		outWavFile.close();
	}
	catch (exception& e) {
		cout << e.what() << endl;
	}

	getchar();
	return 0;

	
}
