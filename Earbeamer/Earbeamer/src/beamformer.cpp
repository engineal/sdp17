#include <iostream>
#include <iomanip>
#include <mutex>
#include <ctime>
#include "beamformer.h"
#include "cexcept.h"

using namespace std;

Beamformer::Beamformer(vector<VirtualSource*> sources) : sources(sources) {
	running = false;
}

Beamformer::~Beamformer() {
	cout << "Beamformer deconstructor" << endl;
}

void Beamformer::start() {
	running = true;
	//spawn processing thread
	beamforming_thread = thread(&Beamformer::beamforming, this);
}

void Beamformer::stop() {
	running = false;
	//spawn processing thread
	beamforming_thread.join();
}

/*
* Create a beam for the target
*/
void Beamformer::updateTargets(map<UINT64, Target*> targets) {
	unique_lock<mutex> lck(beams_mtx);

	for (map<UINT64, Target*>::iterator itr = targets.begin(); itr != targets.end(); ++itr) {
		map<Target*, Beam*>::iterator it = beams.find(itr->second);
		if (it == beams.end()) {
			// Target does not have beam, so add it
			Beam* beam = new Beam(sources);
			beam->update_delays(*(itr->second));
			beams.insert(pair<Target*, Beam*>(itr->second, beam));
		}
		else {
			// Target already has beam, so update it
			it->second->update_delays(*(itr->second));
		}
	}

	map<Target*, Beam*>::iterator itr = beams.begin();
	while (itr != beams.end()) {
		map<UINT64, Target*>::iterator it = targets.find(itr->first->getTrackingId());
		if (it == targets.end()) {
			// Target missing, so remove it
			delete itr->second;
			itr = beams.erase(itr);
		}
		else {
			++itr;
		}
	}
}

bool Beamformer::dataAvailable() {
	unique_lock<mutex> lck(data_buffer_mtx);
	return data_buffer.size() > 0;
}

void Beamformer::waitForData() {
	unique_lock<mutex> lck(data_buffer_mtx);
	while (data_buffer.size() == 0) {
		data_buffer_cv.wait(lck);
	}
}

vector<double> Beamformer::pop_buffer() {
	vector<double> tmp;

	unique_lock<mutex> lck(data_buffer_mtx);
	if (data_buffer.size() > 0) {
		tmp = data_buffer.front();
		data_buffer.pop();
		return tmp;
	}
	throw ElementNotAvailableException();
}

void Beamformer::beamforming() {
	while (running) {
		// Read in samples from sources
		for (int i = 0; i < sources.size(); i++) {
			sources[i]->readBuffer();
		}

		try {
			vector<double> output = calculate_task();
			// push output into buffer
			unique_lock<mutex> lck(data_buffer_mtx);
			data_buffer.push(output);
			data_buffer_cv.notify_all();
		}
		catch (exception& e) {
			cout << e.what() << endl;
		}


	}
}

/**
* Thead to calculate more than one beam
*/
vector<double> Beamformer::calculate_task() {
	vector<double> output;
	output.resize(BUFFER_LENGTH);
	// Clear output buffer first
	for (int i = 0; i < output.size(); i++) {
		output[i] = 0;
	}

	// Calculate each beam separately
	unique_lock<mutex> lck(beams_mtx);
	for (map<Target*, Beam*>::iterator itr = beams.begin(); itr != beams.end(); ++itr) {
		vector<double> temp_output = process_segment(*(itr->second));

		for (int j = 0; j < temp_output.size(); j++) {
			output[j] += temp_output[j];
		}
	}

	// Normalize the audio level so no clipping happens
	for (int i = 0; i < BUFFER_LENGTH; i++) {
		output[i] /= beams.size();
	}
	return output;
}

/**
* Implements delay-sum on one beam
*/
vector<double> Beamformer::process_segment(Beam& beam) {
	vector<double> output;
	output.resize(BUFFER_LENGTH);
	for (int i = 0; i < output.size(); i++) {
		output[i] = 0;
		for (int j = 0; j < sources.size(); j++) {
			output[i] += sources[j]->getSample(i + beam.getDelay(sources[j]));
		}

		output[i] /= sources.size();
	}
	return output;
}