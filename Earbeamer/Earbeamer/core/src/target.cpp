#include "target.h"
#define _USE_MATH_DEFINES
#include <math.h>


/*
*	Constructs the target by passing in a CameraSpacePoint (preferably the head position), and a 
*	body tracking Id.
*/

Target::Target(Coordinate headPosition, UINT64 trackingId) : position(headPosition), trackingId(trackingId)
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

void Target::updatePosition(Coordinate headPosition)
{
	position = headPosition;
}

Coordinate Target::getPosition() {
	return position;
}


/**
*		Calculates the angle of the target from the specified origin
*
*/
double Target::getAngleFromOrigin()
{
	double xCoord = position.x;		
	double yCoord = position.y;

	double thetaRad;
	double thetaDeg;

	if (xCoord == 0) {
		thetaRad = M_PI_2;
	}
	else {
		thetaRad = fabs(atan2(yCoord, xCoord));
	}

	thetaDeg = thetaRad * 180 / M_PI;

	if (xCoord > 0 && yCoord > 0) {
		return thetaDeg;
	}

	else if (xCoord < 0 && yCoord > 0) {
		return 180 - thetaDeg;
	}

	else if (xCoord < 0 && yCoord < 0) {
		return 270 - thetaDeg;
	}

	else {
		return 360 - thetaDeg;
	}

	return thetaDeg;

}
