#include "beam.h"
#include <iomanip>

#define samples_sec 15625.0

using namespace std;

Beam::Beam(vector<VirtualSource*> sources) {
	volume_scalar = 1.0 / sources.size();

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

/**
*	Updating delays using planewave assumption
*
**/

void printMicDelays(vector<VirtualSource*> sources, std::vector<int> delays, double theta) {

	cout << endl;
	cout << "Target Angle: " << theta*180/ 3.1415926535 << endl;

	cout << setw(88) << setfill('#') << endl;
	cout << setfill(' ') << setw(20) << "Low Frequency Array" << endl;
	cout << setw(10) << "Position:";
	for (int i = 0; i < 11; i++) {

		cout << setw(2) << sources[i]->getPosition();
	}
	cout << endl << setw(2) << "Delay:";

	for (int i = 0; i < 11; i++) {

		cout << setw(9) << delays[i];

	}
	cout << endl;
	cout << setfill(' ') << setw(20) << "High Frequency Array" << endl;
	cout << setw(10) << "Position:";
	for (int i = 11; i < 22; i++) {

		cout << setw(2) << sources[i]->getPosition();
	}
	cout << endl << setw(10) << "Delay:";

	for (int i = 11; i < 22; i++) {

		cout << setw(9) << delays[i];

	}

}

void Beam::update_delays(Target target, vector<VirtualSource*> sources) {
	//cout << "Tracking Target" << target.getTrackingId() << " at position" << target.getPosition() << endl;
	double temp = 21; // in C, 70 degrees farenheit
	Coordinate t_coord = target.getPosition();
	double angle_incidence = atan(t_coord.y / t_coord.x);
	double v_sound = 331 + 0.6 * temp; // in m/s
	

	//Determine whether the target is to the left or right of array (assume that target cannot be behind array)
	//Determine which mic is hit first
	double first_x;
	if (angle_incidence >= 0) {
		//Target to the left

		first_x = 0.43;			//Probably shouldn't hardcode these values, but I doubt we'll change them now
	}
	else {
		first_x = -0.43;
	}


	for (int i = 0; i < delays.size(); i++) {



		Coordinate m_coord = sources[i]->getPosition();

		double distance = abs(first_x - m_coord.x)*cos(angle_incidence);
		double t_sound = distance / v_sound; // in s
		delays[i] = (int)((t_sound * samples_sec) + 0.5);		//Add 0.5 to roundup/down correctly (casting to int will truncate)

	}

	//printMicDelays(sources, delays, angle_incidence);

}


/**
 *	Updating delays using spherical delay assumption
 *
 */
/**
void Beam::update_delays(Target target, vector<VirtualSource*> sources) {
	int min_delay = BUFFER_LENGTH;
	for (int i = 0; i < delays.size(); i++) {
		delays[i] = calculate_delay_between_points(target.getPosition(), sources[i]->getPosition());
		if (delays[i] < min_delay) {
			min_delay = delays[i];
		}
	}

	//cout << "Target At X=" << target.getPosition().x << " Y=" << target.getPosition().y << endl;
	//cout << "target delays" << endl;
	// minimize delay on all mics
	for (int i = 0; i < delays.size(); i++) {
		delays[i] -= min_delay;
		//cout << delays[i];
		//if (i == 10) {
		//	cout << endl;
		//} else if (i < delays.size() - 1) {
		//	cout << " ";
		//}
	}
	//cout << endl;
}
*/

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

/**
 *	Set the mute flag to the passed value
 */
void Beam::setMuted(bool mute)
{
	muted = mute;
}

bool Beam::isMuted() {
	return muted;
}

double Beam::getVolumeScalar() {
	return volume_scalar;
}

/*
 * Update the volume scalar
 */
void Beam::updateAvgVolume(double avg_volume) {
	double avg_target_volume = 0.1 / 22;
	// Take average of current volume and target volume so volume change is smoother
	if (avg_volume > 0.02) {
		volume_scalar = (volume_scalar + (avg_target_volume / avg_volume)) / 2;
	}

	//cout << "avg_volume: " << avg_volume << ", scalar: " << volume_scalar << endl;
}
