#pragma once

#include "portaudio.h"
#include <thread>
#include <mutex>
#include <queue>


using namespace std;

class OutputDevice {

public:

	OutputDevice(int amplify, int rate);
	~OutputDevice();

	void connect();
	void disconnect();

	//bool deviceEmpty();						//Returns true if device queue has no data

	void enqueue(vector<double> block);		//Queue a block of data to be played			

	std::mutex queue_access;				//Used to control access to targets
	std::condition_variable queue_update;	//Used to notify when targets have been updated


private:

	void feed_output();		//thread that continually feeds output device, as long as data is available in queue

	PaStream* stream;

	std::queue<std::vector<double>> data_buffer;
	int amplification;
	int sample_rate;		//The sample rate of data to be played through output
	thread t_output;
	bool running;
	PaStreamParameters outputParameters;
	PaStreamParameters inputParameters;


};