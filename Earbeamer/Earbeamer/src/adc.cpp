#include <math.h>
#include <NIDAQmx.h>
#include "adc.h"

using namespace std;

#define DAQmxErrChk(functionCall) if(DAQmxFailed(functionCall)){ DAQmxGetExtendedErrorInfo(errBuff, 2048);printf("DAQmx Error: %s\n", errBuff);}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);

queue<pair<float64*, int>> data_buffer;

/*-----------------------------------------------------------------------------
 * ADC
 *-----------------------------------------------------------------------------*/

ADC::ADC(string fileName) {
	int32       error = 0;
	char        errBuff[2048] = { '\0' };

	DAQmxErrChk(DAQmxCreateTask("", &taskHandle));
	DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandle, "Dev1/ai1", "", DAQmx_Val_RSE, 0.0, 3.0, DAQmx_Val_Volts, NULL));
	DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle, "", 16000.0, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 1024));

	DAQmxErrChk(DAQmxRegisterEveryNSamplesEvent(taskHandle, DAQmx_Val_Acquired_Into_Buffer, 1000, 0, EveryNCallback, NULL));
	DAQmxErrChk(DAQmxRegisterDoneEvent(taskHandle, 0, DoneCallback, NULL));

	DAQmxErrChk(DAQmxStartTask(taskHandle));

	printf("Acquiring samples continuously.\n");
}

ADC::~ADC() {
}

int ADC::dataAvailable() {
	return data_buffer.size() > 0;
}

int ADC::readBuffer(double* samples, int n) {
	pair<float64*, int> pair = data_buffer.front();
	data_buffer.pop();
	for (int i = 0; i < pair.second; i++) {
		samples[i] = pair.first[i] * 32768;
	}
	delete pair.first;
	return pair.second;
}

void ADC::close() {
	if (taskHandle != 0) {
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	cout << "everythings done" << endl;
}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
	int32       error = 0;
	char        errBuff[2048] = { '\0' };
	static int  totalRead = 0;
	int32       read = 0;
	float64*    tmp_data = new float64[1024];

	/*********************************************/
	// DAQmx Read Code
	/*********************************************/
	DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, 1024, 10.0, DAQmx_Val_GroupByScanNumber, tmp_data, 1024, &read, NULL));
	if (read>0) {
		pair<float64*, int> pair;
		pair.first = tmp_data;
		pair.second = read;
		data_buffer.push(pair);
		printf("Acquired %d samples. Total %d\n", (int)read, (int)(totalRead += read));
		fflush(stdout);
	}
	return 0;
}

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData)
{
	int32   error = 0;
	char    errBuff[2048] = { '\0' };

	// Check to see if an error stopped the task.
	DAQmxErrChk(status);
	return 0;
}
