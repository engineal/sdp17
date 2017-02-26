#include "listener.h"

IListener::IListener() {
	_hasRead = false;
}

bool IListener::hasRead() {
	return _hasRead;
}

void IListener::setRead() {
	_hasRead = true;
}

void IListener::clearRead() {
	_hasRead = false;
}