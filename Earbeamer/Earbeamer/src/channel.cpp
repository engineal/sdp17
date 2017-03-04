#include <iostream>
#include "channel.h"
#include "cexcept.h"

using namespace std;

Channel::Channel(string channel_id) : channel_id(channel_id) {}

Channel::~Channel() {
	cout << "Channel deconstructor" << endl;
}

string Channel::getChannelId() {
	return channel_id;
}

void Channel::addListener(IListener* listener) {
	unique_lock<mutex> lck(listeners_mtx);
	listeners.push_back(listener);
}

void Channel::removeListener(IListener* listener) {
	unique_lock<mutex> lck(listeners_mtx);
	listeners.erase(remove(listeners.begin(), listeners.end(), listener));

	if (everyoneRead()) {
		data_buffer.pop();
		for (vector<IListener*>::iterator itr = listeners.begin(); itr != listeners.end(); ++itr) {
			(*itr)->clearRead();
		}
		listeners_cv.notify_all();
	}
}

bool Channel::dataAvailable() {
	unique_lock<mutex> lck(data_buffer_mtx);
	return data_buffer.size() > 0;
}

void Channel::waitForData() {
	unique_lock<mutex> lck(data_buffer_mtx);
	while (data_buffer.size() == 0) {
		data_buffer_cv.wait(lck);
	}
}

void Channel::push_buffer(vector<double> data) {
	unique_lock<mutex> lck(data_buffer_mtx);
	data_buffer.push(data);
	data_buffer_cv.notify_all();
}

bool Channel::everyoneRead() {
	bool result = true;
	for (vector<IListener*>::iterator itr = listeners.begin(); itr != listeners.end(); ++itr) {
		result &= (*itr)->hasRead();
	}
	return result;
}

vector<double> Channel::pop_buffer(IListener* listener) {
	unique_lock<mutex> data_lck(data_buffer_mtx);
	if (data_buffer.size() > 0) {
		unique_lock<mutex> listen_lck(listeners_mtx);
		// wait if listener has already read current buffer
		if (listener->hasRead()) {
			data_lck.release();
			cout << "waiting for all listeners" << endl;
			listeners_cv.wait(listen_lck);
			data_lck.lock();
			cout << "all listeners done" << endl;
		}
		vector<double> data = data_buffer.front();
		listener->setRead();

		// Check if more listeners need the data and if not, remove it from the queue
		if (everyoneRead()) {
			data_buffer.pop();
			for (vector<IListener*>::iterator itr = listeners.begin(); itr != listeners.end(); ++itr) {
				(*itr)->clearRead();
			}
			listeners_cv.notify_all();
		}
		return data;
	}
	throw ElementNotAvailableException();
}