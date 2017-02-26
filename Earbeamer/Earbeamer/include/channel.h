#pragma once

#include <queue>
#include <vector>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include "listener.h"

class Channel {
private:
	std::string channel_id;
	std::vector<IListener*> listeners;
	int listeners_read;
	std::mutex data_buffer_mtx;
	std::condition_variable data_buffer_cv;
	std::mutex listeners_mtx;
	std::condition_variable listeners_cv;
public:
	std::queue<std::vector<double>> data_buffer;
	Channel(std::string channel_id);
	~Channel();
	std::string getChannelId();
	void addListener(IListener* listener);
	void removeListener(IListener* listener);
	bool dataAvailable();
	void waitForData();
	void push_buffer(std::vector<double> data);
	std::vector<double> pop_buffer(IListener* listener);
};