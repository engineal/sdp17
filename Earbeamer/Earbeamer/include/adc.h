#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <utility>
#include <NIDAQmx.h>

class ADC {
protected:
	TaskHandle taskHandle = 0;
public:
	ADC(std::string fileName);
	~ADC();
	int dataAvailable();
	int readBuffer(double* samples, int n);
	int callback();
	void close();
};
