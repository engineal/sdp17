#include <iostream>
#include "channel.h"
#include "cexception.h"

using namespace std;

Channel::Channel(string channel_id) : channel_id(channel_id){
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
	listeners++;
}

bool Channel::dataAvailable() {
	return data_buffer.size() > 0;
}

void Channel::push_buffer(double* data, int size) {
	data_buffer.push(pair<double*, int>(data, size));
}

pair<double*, int> Channel::pop_buffer() {
	if (data_buffer.size() > 0) {
		pair<double*, int> tmp = data_buffer.front();
		if ((++listeners_read) == listeners) {
			data_buffer.pop();
			listeners_read = 0;
		}
		return tmp;
	}
	throw ElementNotAvailableException();
}