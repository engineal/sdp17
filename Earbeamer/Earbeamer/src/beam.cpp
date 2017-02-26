#include "beam.h"

#define samples_sec 16000

using namespace std;

Beam::Beam(vector<VirtualSource*> sources) {
	for (vector<VirtualSource*>::iterator itr = sources.begin(); itr != sources.end(); ++itr) {
		delays.insert(pair<VirtualSource*, int>(*itr, 0));
	}
}

Beam::~Beam() {
	cout << "Beam deconstructor" << endl;
}

int Beam::getDelay(VirtualSource* source) {
	return delays.find(source)->second;
}

void Beam::update_delays(Target target) {
	int min_delay = BUFFER_LENGTH;
	for (map<VirtualSource*, int>::iterator itr = delays.begin(); itr != delays.end(); ++itr) {
		itr->second = calculate_delay_between_points(target.getPosition(), itr->first->getPosition());
		if (itr->second < min_delay) {
			min_delay = itr->second;
		}
	}

	//cout << "target delays" << endl;
	// minimize delay on all mics
	for (map<VirtualSource*, int>::iterator itr = delays.begin(); itr != delays.end(); ++itr) {
		itr->second -= min_delay;
		//cout << itr->second << endl;
	}
	//cout << endl;
}

/*
* Calculate a delay for the source based on target's coordinates
*/
int Beam::calculate_delay_between_points(Coordinate coord1, Coordinate coord2) {
	double temp = 20; // in C

	double distance = sqrt(pow(coord1.x - coord2.x, 2) + pow(coord1.y - coord2.y, 2)); // in m
	double v_sound = 331 + 0.6 * temp; // in m/s
	double t_sound = distance / v_sound; // in s
	return (int)((t_sound * samples_sec) + 0.5); // in samples
}
