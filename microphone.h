#ifndef MICROPHONE_H
#define MICROPHONE_H

class Microphone {
protected:
	double x_pos; // absolute x position in m
	double y_pos; // absolute y position in m

public:
	double* buffA;
	double* buffB;
	double* buffC;
	int delay;
	
	Microphone(double x, double y);
	~Microphone();
	void read_sample();
	void rotate_buffers();
	void calculate_delay(double x, double y);
};

#endif /*MICROPHONE_H*/