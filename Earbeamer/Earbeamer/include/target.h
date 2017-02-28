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

	BOOLEAN isMuted();
	BOOLEAN getTrackedStatus();
	Coordinate getPosition();
	UINT64 getTrackingId();
	double getAngleFromOrigin();

	/**
	 *	Overloading ostream operator, to print JSON representation of target
	 *	Used to publish updates over websocket connection
	 */
	friend ostream& operator<<(ostream& os, Target  targ)
	{
		Coordinate pos = targ.getPosition();
		os << "{\"id\": " << targ.getTrackingId() << ',';
		os << "\"x_coord\": " << pos.x << ',';
		os << "\"y_coord\": " << pos.y << ',';
		os << "\"muted\": ";
		if (targ.isMuted()) {
			os << "true}";
		}
		else {
			os << "false}";
		}
		return os;
	}

	
	
private:
	BOOLEAN tracked;
	BOOLEAN muted;
	UINT64 trackingId;
	UINT8 unmuted_volume;
	UINT8 volume;
	Coordinate position;
};