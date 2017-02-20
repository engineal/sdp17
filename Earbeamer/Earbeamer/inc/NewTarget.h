#pragma once
#pragma once


#include <stdint.h>
#include <Windows.h>
#include <Kinect.h>
class NewTarget: public IBody {


public:

	Target(CameraSpacePoint headPosition, UINT64 trackingId);


	void togglemute();
	void setVolume(int volume);

	void setTracked(BOOLEAN status);
	BOOLEAN getTrackedStatus();

	void updatePosition(CameraSpacePoint headPosition);

	UINT64 getTrackingId();
	float getAngleFromReference();



private:

	BOOLEAN tracked;
	BOOLEAN muted;
	UINT64 trackingId;
	UINT8 unmuted_volume;
	UINT8 volume;
	CameraSpacePoint position;
};