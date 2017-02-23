#include <iostream>
#include <math.h>
#include "adc.h"
#include "cexception.h"

using namespace std;

#define DAQmxErrChk(functionCall) if(DAQmxFailed(functionCall)){ throw ADCException(); }

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);

ADC::ADC(vector<Channel*> channels, double rate) : channels(channels), rate(rate) {
	tmp_data_size = 1024 * (int) channels.size();

	DAQmxErrChk(DAQmxCreateTask("", &taskHandle));
	for (vector<Channel*>::iterator itr = channels.begin(); itr != channels.end(); ++itr) {
		DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandle, (*itr)->getChannelId().c_str(), "", DAQmx_Val_RSE, 0.0, 3.0, DAQmx_Val_Volts, NULL));
	}
	DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle, "", rate, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 1024));

	DAQmxErrChk(DAQmxRegisterEveryNSamplesEvent(taskHandle, DAQmx_Val_Acquired_Into_Buffer, 1000, 0, EveryNCallback, this));
	DAQmxErrChk(DAQmxRegisterDoneEvent(taskHandle, 0, DoneCallback, this));

	running = false;
}

ADC::~ADC() {
	cout << "ADC deconstructor" << endl;
	if (running) {
		stop();
	}

	if (taskHandle != 0) {
		DAQmxClearTask(taskHandle);
	}
}

void ADC::start() {
	DAQmxErrChk(DAQmxStartTask(taskHandle));
	running = true;
}

void ADC::stop() {
	if (taskHandle != 0) {
		DAQmxStopTask(taskHandle);
		running = false;
	}
}

void ADC::data_callback() {
	static int  totalRead = 0;
	int32       read = 0;
	float64*    tmp_data = new float64[tmp_data_size];

	DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, 1024, 10.0, DAQmx_Val_GroupByChannel, tmp_data, tmp_data_size, &read, NULL));

	if (read>0) {
		int channel_segment_length = read / (int) channels.size();
		//separate channels
		for (int i = 0; i < channels.size(); i++) {
			channels[i]->push_buffer(tmp_data + (i * channel_segment_length), channel_segment_length);
		}

		cout << "Acquired " << read << " samples. Total " << (totalRead += read) << endl;
	}
}

void ADC::done_callback(int32 status) {
	// Check to see if an error stopped the task.
	DAQmxErrChk(status);
}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData) {
	ADC* self = static_cast<ADC*>(callbackData);
	self->data_callback();
	
	return 0;
}

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData) {
	ADC* self = static_cast<ADC*>(callbackData);
	self->done_callback(status);

	return 0;
}
