
#include <iostream>
#include "outputdevice.h"
#include "cexcept.h"

using namespace std;

/**
 *	Constructs an output device with an amplification factor and a sample rate
 */
OutputDevice::OutputDevice(int amplify, int rate) {
	this->amplification = amplify;
	this->running = false;
	this->sample_rate = rate;

	PaError err = Pa_Initialize();
	if (err) {
		throw OutputDeviceException(err);
	}

	int numDevices;
	numDevices = Pa_GetDeviceCount();
	if (numDevices < 0) {
		std::cout << "No output devices detected" << endl;
		throw OutputDeviceException(err);
	}

	//Initialize the parameters for output
	//Multiple channels currently not supported
	outputParameters.device = Pa_GetDefaultOutputDevice();
	outputParameters.channelCount = 1;
	outputParameters.sampleFormat = paInt16;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultHighOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;


}

OutputDevice::~OutputDevice() {

}

/**
 *		Adds a data block to the queue, which will be directed to the audio output by the worker thread
 */

void OutputDevice::enqueue(vector<double> block)
{
	unique_lock<std::mutex> lck(queue_access);
	
	this->data_buffer.push(block);

	queue_update.notify_one();

}

void OutputDevice::feed_output() {

	while (this->running) {
		try {
			unique_lock<std::mutex> lck(queue_access);

			//If data buffer is empty, wait until something has been queued
			if (this->data_buffer.size() == 0) {
				this->queue_update.wait(lck);
			}

			if (this->running) {
				vector<double> block_dub = data_buffer.front();
				short* block_shr = new short[block_dub.size()];


				for (int ii = 0; ii < block_dub.size(); ii++)
				{
					block_shr[ii] = (short)(block_dub[ii] * amplification);
				}

				//Otherwise load up the device
				PaError err = Pa_WriteStream(this->stream, block_shr, block_dub.size());
				if (err) {
					throw OutputDeviceException(err);
				}

				//Deallocate Memory
				delete[] block_shr;
				data_buffer.pop();
			}
		}
		catch (exception& e) {
			cout << e.what() << endl;
			running = false;
		}
	}

}

void OutputDevice::connect() {

	

	PaError err = Pa_OpenStream(
		&stream,
		NULL,			/*No input parameters needed, not reading from laptop microphones*/
		&outputParameters,
		this->sample_rate,
		paFramesPerBufferUnspecified,
		paClipOff,      /* we won't output out of range samples so don't bother clipping them */
		NULL,			/* no callback, use blocking API */
		NULL);			/* no callback, so no callback userData */

	if (err != paNoError) {
		goto error;
	}

	err = Pa_StartStream(stream);
	if (err != paNoError) {
		goto error;
	}

	running = true;
	this->t_output = thread(&OutputDevice::feed_output, this);

	return;
error:
	throw OutputDeviceException(err);


}
/**
 *	Stop the stream and clean up
 */

void OutputDevice::disconnect() {

	running = false;
	queue_update.notify_all();
	if (this->t_output.joinable()) {
		this->t_output.join();
	}


	PaError err = Pa_StopStream(stream);
	if (err != paNoError) {
		goto error;
	}

	err = Pa_CloseStream(stream);
	if (err != paNoError) {
		goto error;
	}

	err = Pa_Terminate();
	if (err != paNoError) {
		goto error;
	}
	return;
error:
	throw OutputDeviceException(err);

}