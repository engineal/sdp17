#pragma once

#include <utility>
#include <queue>
#include <string>
#include <mutex>

class Channel {
private:
	std::queue<std::pair<double*, int>> data_buffer;
	std::string channel_id;
	int listeners;
	int listeners_read;
	std::mutex data_buffer_mtx;
	std::mutex listeners_mtx;
public:
	Channel(std::string channel_id);
	~Channel();
	std::string getChannelId();
	void addListener();
	bool dataAvailable();
	void push_buffer(double* data, int size);
	std::pair<double*, int> pop_buffer();
};