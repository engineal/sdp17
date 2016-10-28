#include "microphone.h"

#ifndef BEAMFORMING_H
#define BEAMFORMING_H

#define NUM_MICS 16
#define BUFFER_LENGTH 1024

void calculate_delays(Microphone** mic);
int process_segment(Microphone** mics, char* output);
void read_sample(Microphone* mic);

#endif /*BEAMFORMING_H*/