#include "beam.h"

#define samples_sec 15625.0

using namespace std;

Beam::Beam(vector<VirtualSource*> sources) {
	for (vector<VirtualSource*>::iterator itr = sources.begin(); itr != sources.end(); ++itr) {
		delays.push_back(0);
	}
}

Beam::~Beam() {
	cout << "Beam deconstructor" << endl;
}

int Beam::getDelay(int source) {
	return delays[source];
}

void Beam::update_delays(Target target, vector<VirtualSource*> sources) {
	int min_delay = BUFFER_LENGTH;
	for (int i = 0; i < delays.size(); i++) {
		delays[i] = calculate_delay_between_points(target.getPosition(), sources[i]->getPosition());
		if (delays[i] < min_delay) {
			min_delay = delays[i];
		}
	}

	cout << "Target At X=" << target.getPosition().x << " Y=" << target.getPosition().y << endl;
	cout << "target delays" << endl;
	// minimize delay on all mics
	for (int i = 0; i < delays.size(); i++) {
		delays[i] -= min_delay;
		cout << delays[i];
		if (i == 10) {
			cout << endl;
		} else if (i < delays.size() - 1) {
			cout << " ";
		}
	}
	cout << endl;
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
