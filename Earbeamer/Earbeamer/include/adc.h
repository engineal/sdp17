#include <iostream>
#include <string>
#include <queue>
#include <utility>
#include <NIDAQmx.h>

#ifndef ADC_H
#define ADC_H

class ADC {
protected:
	TaskHandle taskHandle = 0;
public:
	ADC(std::string fileName);
	~ADC();
	int dataAvailable();
	int readBuffer(double* samples, int n);
	void close();
};

#endif // ADC_H
