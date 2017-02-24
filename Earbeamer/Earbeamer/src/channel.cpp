#include <iostream>
#include "channel.h"
#include "cexception.h"

using namespace std;

Channel::Channel(string channel_id) : channel_id(channel_id){
	listeners_mtx.lock();
	listeners = 0;
	listeners_mtx.unlock();

	listeners_read = 0;
}

Channel::~Channel() {
	cout << "Channel deconstructor" << endl;
}

string Channel::getChannelId() {
	return channel_id;
}

void Channel::addListener() {
	listeners_mtx.lock();
	listeners++;
	listeners_mtx.unlock();
}

bool Channel::dataAvailable() {
	data_buffer_mtx.lock();
	return data_buffer.size() > 0;
	data_buffer_mtx.unlock();
}

void Channel::push_buffer(double* data, int size) {
	data_buffer_mtx.lock();
	data_buffer.push(pair<double*, int>(data, size));
	data_buffer_mtx.unlock();
}

pair<double*, int> Channel::pop_buffer() {
	pair<double*, int> tmp;

	data_buffer_mtx.lock();
	if (data_buffer.size() > 0) {
		tmp = data_buffer.front();
		listeners_mtx.lock();
		if ((++listeners_read) == listeners) {
			data_buffer.pop();
			listeners_read = 0;
		}
		listeners_mtx.unlock();
		data_buffer_mtx.unlock();
		return tmp;
	}
	data_buffer_mtx.unlock();
	throw ElementNotAvailableException();
}