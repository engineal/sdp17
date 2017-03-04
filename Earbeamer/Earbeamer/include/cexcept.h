#pragma once

#include <exception>

class ElementNotAvailableException : public std::exception {
	virtual const char* what() const throw();
};

class ADCException : public std::exception {
	virtual const char* what() const throw();
public:
	ADCException();
	~ADCException();
private:
	char* errBuff;
};

class OutputDeviceException : public std::exception {
	virtual const char* what() const throw();

public:
	OutputDeviceException(int err);
private:
	int error;;
};


