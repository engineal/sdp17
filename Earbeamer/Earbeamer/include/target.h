#pragma once


#include <stdint.h>
#include <Windows.h>
#include <Kinect.h>

#include "coordinate-system.h"

class Target {


public:

	Target(Coordinate headPosition, UINT64 trackingId);


	void togglemute();
	void setVolume(int volume);
	void setTracked(BOOLEAN status);
	void updatePosition(Coordinate headPosition);


	BOOLEAN getTrackedStatus();
	Coordinate getPosition();
	UINT64 getTrackingId();
	double getAngleFromOrigin();

	
	
private:

	BOOLEAN tracked;
	BOOLEAN muted;
	UINT64 trackingId;
	UINT8 volume;
	Coordinate position;
};