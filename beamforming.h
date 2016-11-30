#include <string>
#include "microphone.h"
#include "wav-file.h"

#ifndef BEAMFORMING_H
#define BEAMFORMING_H

#define NUM_MICS 16
#define BUFFER_LENGTH 1024

void calculate_delays(Microphone** mic, double x, double y);
void read_inputs(Microphone** mics, iWavFile* source1, iWavFile* source2);
void rotate_buffers(Microphone** mics);

void calculate_task(Microphone** mics, double* output);
void process_segment(Microphone** mics, double* output);

#endif /*BEAMFORMING_H*/