#pragma once

#ifndef COORD_SYSTEM_H
#define COORD_SYSTEM_H

#include <iostream>
#include <Kinect.h>

using namespace std;

struct Coordinate {
	/**
	*	Struct containing coordinates based upon the center of the microphone array
	*	X Axis is parallel to array, Y axis is perpendicular
	*
	*	@ = Microphone
	*											|Y
	*											|
	*											|
	*											|
	*											|____________________________X
	*		@			@			@			@			@			@			@
	*/
public: 
	Coordinate(double x_coord, double y_coord) {
		x = x_coord;
		y = y_coord;
	}


	double x;
	double y;

	friend ostream& operator<<(ostream& os, Coordinate  coord)
	{
		os << '(' << coord.x << ',' << coord.y << ')';
		return os;
	}


};

/**
*	Class is used to represent a new coordinate system based upon a linear segment 
*	in Kinect Camera space. Once defined, CoordinateSystem may be used to map 
*	from CameraSpace to the new coordinate system
*
*/
class CoordinateSystem {

private:
	CameraSpacePoint origin;		//The origin of the coordinate system, in camera space
	double orientation;				//The counterclockwise angular displacement of the X-Axis, from 
									//the cameraspace x-axis

public:
	CoordinateSystem(CameraSpacePoint leftEnd, CameraSpacePoint rightEnd);
	Coordinate cameraToSystemSpace(CameraSpacePoint);
	double getOrientation();
	CameraSpacePoint getOrigin();
};


#endif