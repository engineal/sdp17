#include <cmath>
#include <iostream>

#include "coordinate-system.h"

# define M_PI           3.14159265358979323846  /* pi */


CoordinateSystem::CoordinateSystem(CameraSpacePoint left, CameraSpacePoint right)
{
	origin = CameraSpacePoint();
	origin.X = (left.X + right.X) / 2;
	origin.Z = (left.Z + right.Z) / 2;
	origin.Y = origin.Y;

	//Determine Orientation Angle of Microphone Array, in radians
	//(Starting from the X Axis of the Kinect, what is the counterclockwise 
	//angular displacement of the microphone array?)
	
	//Determine Vector to represent orientation of Microphone Array, in CameraSpace Coordinate System
	double x0 = right.X - left.X;
	double y0 = right.Z - left.Z;

	if (x0 < 0 && y0 > 0) {
		//Vector Represention of Array (pointing from left end to right end) is located in Quadrant II
		orientation = M_PI - fabs(atan(y0 / x0));
	}
	else if (x0 > 0 && y0 > 0) {
		//Vector is located in Quadrant I

		orientation = atan(y0 / x0);
	}
	else if (x0 < 0 && y0 < 0) {
		//Vector is located in Quadrant III

		orientation = (3 / 2)*M_PI - fabs(atan(y0 / x0));
	}
	else {
		//Vector is located in Quadrant IV

		orientation = 2 * M_PI - fabs(atan(y0 / x0));
	}


}

Coordinate CoordinateSystem::cameraToSystemSpace(CameraSpacePoint pt)
{

	//Applies a Coordinate System Translation, then a rotation
	std::cout << orientation << std::endl;
	double x_prime = (pt.X - origin.X)*cos(orientation) + (pt.Z - origin.Z)*sin(orientation);
	double z_prime = -1 * (pt.X - origin.X)*sin(orientation) + (pt.Z - origin.Z)*cos(orientation);
	return Coordinate(x_prime, z_prime);
}

CameraSpacePoint CoordinateSystem::getOrigin() {
	return origin;
}

double CoordinateSystem::getOrientation() {
	return orientation;
}


