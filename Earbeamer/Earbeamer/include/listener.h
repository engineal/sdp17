#pragma once

class IListener {
private:
	bool _hasRead;
public:
	IListener();
	bool hasRead();
	void setRead();
	void clearRead();
};