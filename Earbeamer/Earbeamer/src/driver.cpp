
#include <iostream>
#include "websocket_server.h"// <--- Must be included before windows.h
#include <windows.h>
#include <time.h>
#include "channel.h"
#include "virtual_source.h"
#include <conio.h>

#include "adc.h"
#include "beamformer.h"
#include "wav-file.h"
#include "room.h"
#include "outputdevice.h"
#include <map>

using namespace std;

double CHEBYSHEV_WEIGHTS_20DB[11] = { 1.0000, 0.8450, 1.0949, 1.3036, 1.4422, 1.4907, 1.4422, 1.3036, 1.0949, 0.8450, 1.0000 };
double UNIFORM_WEIGHTS[11] = { 1,1,1,1,1,1,1,1,1,1,1 };

bool getInput(char *c)
{
	if (_kbhit())
	{
		*c = _getch();
		return true; // Key Was Hit
	}
	return false; // No keys were pressed
}


vector<VirtualSource*> createSources(vector<Channel*> channels) {
	vector<pair<double, Channel*>> low_positions = {
		pair<double, Channel*>(-0.43, channels[0]),
		pair<double, Channel*>(-0.344, channels[1]),
		pair<double, Channel*>(-0.258, channels[2]),
		pair<double, Channel*>(-0.172, channels[4]),
		pair<double, Channel*>(-0.086, channels[6]),
		pair<double, Channel*>(0, channels[8]),
		pair<double, Channel*>(0.086, channels[10]),
		pair<double, Channel*>(0.172, channels[12]),
		pair<double, Channel*>(0.258, channels[13]),
		pair<double, Channel*>(0.344, channels[14]),
		pair<double, Channel*>(.43, channels[15])
	};
	vector<pair<double, Channel*>> high_positions = {
		pair<double, Channel*>(-.258, channels[2]),
		pair<double, Channel*>(-.215, channels[3]),
		pair<double, Channel*>(-.172, channels[4]),
		pair<double, Channel*>(-.129, channels[5]),
		pair<double, Channel*>(-.086, channels[6]),
		pair<double, Channel*>(-.043, channels[7]),
		pair<double, Channel*>(0, channels[8]),
		pair<double, Channel*>(.043, channels[9]),
		pair<double, Channel*>(.086, channels[10]),
		pair<double, Channel*>(.129, channels[11]),
		pair<double, Channel*>(.172, channels[12])
	};

	vector<VirtualSource*> sources;
	for (vector<pair<double, Channel*>>::iterator itr = low_positions.begin(); itr != low_positions.end(); ++itr) {
		sources.push_back(new VirtualSource(itr->second, Coordinate(itr->first, 0), LOW, CHEBYSHEV_WEIGHTS_20DB[std::distance(low_positions.begin(), itr)]));
	}
	for (vector<pair<double, Channel*>>::iterator itr = high_positions.begin(); itr != high_positions.end(); ++itr) {
		sources.push_back(new VirtualSource(itr->second, Coordinate(itr->first, 0), HIGH, CHEBYSHEV_WEIGHTS_20DB[std::distance(high_positions.begin(), itr)]));
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
	left.X = 0.43f;
	left.Y = 0.0f;
	left.Z = .0762f;

	CameraSpacePoint right = CameraSpacePoint();
	right.X = -0.43f;
	right.Y = 0.0f;
	right.Z = 0.0762f;

	CoordinateSystem grid = CoordinateSystem(left, right);

	try {
		ADC adc(channels, 15625.0);
		Room room(grid);
		room.Init();
		
		Beamformer beamformer(sources);
		room.beginMonitoring(&beamformer);

		OutputDevice speaker(200000, 15625);	//Create an OutputDevice with an amplification factor of 1, sample rate of 15,000 Hz
		speaker.connect();

		WebsocketServer server(room);

		//Generate some test targets
		//room->DEBUG_GenerateTestTarget();
		//Sleep(1000);
		//room->DEBUG_GenerateTestTarget();

		server.run(9002);
		server.begin_broadcast();
		/**
		time_t start;
		time_t current;
		time(&start);
		time(&current);

		while (difftime(current, start) < 99999) {
			Sleep(1000);
			Room::target_trigger.notify_one();
			time(&current);
		}

		server.stop();
		**/
		
		oWavFile outWavFile("test.wav", 750000);
		oWavFile* outputFiles[16];
		IListener listeners[16];
		for (int i = 0; i < channels.size(); i++) {
			outputFiles[i] = new oWavFile("test" + to_string(i) + ".wav", 200000);
			channels[i]->addListener(&listeners[i]);
		}

		time_t start;
		time_t current;

		adc.start();
		beamformer.start();
		
		time(&start);
		time(&current);
		
		bool running = true;
		while (difftime(current, start) < 60*20 && running) {
			for (int i = 0; i < 16; i++) {
				channels[i]->waitForData();
				vector<double> data = channels[i]->pop_buffer(&listeners[i]);
				outputFiles[i]->writeBuffer(data);
			}

			beamformer.waitForData();
			vector<double> output = beamformer.pop_buffer();

			speaker.enqueue(output);
			outWavFile.writeBuffer(output);
			
			//cout << difftime(current, start) << endl;
			time(&current);

			char key = ' ';
			if (getInput(&key) && key == 'q') {
				running = false;
			}
		}

		outWavFile.close();
		for (int i = 0; i < channels.size(); i++) {
			outputFiles[i]->close();
			channels[i]->removeListener(&listeners[i]);
		}

		speaker.disconnect();
		server.stop();
		room.stop();
		beamformer.stop();
		adc.stop();	
	}
	catch (exception& e) {
		cout << e.what() << endl;
	}

	cout << "Done!" << endl;

	getchar();
	return 0;
}
