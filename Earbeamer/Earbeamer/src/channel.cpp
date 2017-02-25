#include <iostream>
#include "channel.h"
#include "cexcept.h"

using namespace std;

Channel::Channel(string channel_id) : channel_id(channel_id){
	unique_lock<mutex> lck(listeners_mtx);
	listeners = 0;

	listeners_read = 0;
}

Channel::~Channel() {
	cout << "Channel deconstructor" << endl;
}

string Channel::getChannelId() {
	return channel_id;
}

void Channel::addListener() {
	unique_lock<mutex> lck(listeners_mtx);
	listeners++;
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

vector<double> Channel::pop_buffer() {
	unique_lock<mutex> lck(data_buffer_mtx);
	if (data_buffer.size() > 0) {
		vector<double> data = data_buffer.front();

		// Check if more listeners need the data and if not, remove it from the queue
		unique_lock<mutex> lck(listeners_mtx);
		if ((++listeners_read) == listeners) {
			data_buffer.pop();
			listeners_read = 0;
		}
		return data;
	}
	throw ElementNotAvailableException();
}