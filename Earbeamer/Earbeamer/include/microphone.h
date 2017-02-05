#ifndef MICROPHONE_H
#define MICROPHONE_H

class Microphone {
protected:
	int samples_sec;
	double x_pos; // absolute x position in m
	double y_pos; // absolute y position in m

public:
	double* buffA;
	double* buffB;
	double* buffC;
	int delay;
	
	Microphone(int samples_rate, double x, double y);
	~Microphone();
	void read_sample();
	void rotate_buffers();
	int calculate_delay_to_point(double x, double y);
};

#endif /*MICROPHONE_H*/