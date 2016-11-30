#include "target.h"
#define _USE_MATH_DEFINES
#include <math.h>


/*
*	Constructs the target by passing in a CameraSpacePoint (preferably the head position), and a 
*	body tracking Id.
*/

Target::Target(CameraSpacePoint headPosition, UINT64 trackingId) : position(headPosition),
trackingId(trackingId)
{
	tracked = true;
	muted = false;
	volume = 100;
	unmuted_volume = 100;

}

void Target::togglemute() {
	if (muted) {
		volume = unmuted_volume;
		muted = false;
	}
	else {
		unmuted_volume = volume;
		volume = 0;
		muted = true;
	}
}

void Target::setVolume(int newVol) {
	volume = newVol;
}

void Target::setTracked(BOOLEAN status) {
	tracked = status;
}

BOOLEAN Target::getTrackedStatus()
{
	return tracked;
}

UINT64 Target::getTrackingId()
{
	return trackingId;
}

void Target::updatePosition(CameraSpacePoint headPosition)
{
	position = headPosition;
}


/**
*		Calculates the angle from a given reference point
*
*		TODO: Calculate the angle from a CameraSpacePoint other than {x,z} = {0,0}
*/
float Target::getAngleFromReference()
{
	float xCoord = position.X * -1;		//XCoordinates are mirrored, so multiply by negative 1
	float zCoord = position.Z;

	double thetaRad;
	double thetaDeg;

	if (xCoord == 0) {
		thetaRad = M_PI_2;
	}
	else {
		thetaRad = atan2(zCoord, xCoord);
	}

	thetaDeg = thetaRad * 180 / M_PI;

	if (thetaDeg < 0) {
		thetaDeg = 180 - thetaDeg;
	}

	return thetaDeg;

}
