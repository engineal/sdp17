#include <iostream>
#include <NIDAQmx.h>
#include "cexception.h"

using namespace std;

const char* ElementNotAvailableException::what() const throw() {
	return "No elements exist";
}

ADCException::ADCException() {
	errBuff = new char[2048];
	errBuff[0] = '\0';
}

ADCException::~ADCException() {
	cout << "ADCException deconstructor" << endl;
	delete[] errBuff;
}

const char* ADCException::what() const throw() {
	DAQmxGetExtendedErrorInfo(errBuff, 2048);
	return errBuff;
}