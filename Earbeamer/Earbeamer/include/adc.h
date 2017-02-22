#pragma once

#include <vector>
#include <NIDAQmx.h>
#include "channel.h"

class ADC {
private:
	TaskHandle taskHandle = 0;
	int tmp_data_size;
	std::vector<Channel> channels;
	double rate;
	bool running;
public:
	ADC(std::vector<Channel> channels, double rate);
	~ADC();
	void start();
	void stop();
	void data_callback();
	void done_callback(int32 status);
};
