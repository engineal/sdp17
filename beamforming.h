#ifndef BEAMFORMING_H
#define BEAMFORMING_H

#define NUM_MICS 16
#define SAMPLE_SIZE 1024

void calculate_delays(char* delays);
int process_segment();
int read_sample();

#endif /*BEAMFORMING_H*/