// Tests.cpp : Defines the entry point for the console application.
//

#include "gtest/gtest.h"
#include "../arithmetic.h"
#include "coordinate-system.h"
#include <Kinect.h>
#include <iostream>

#define ALLOWED_ERR 0.0001

using namespace std;


TEST(SquareTests, Square)
{
	EXPECT_EQ(0, square(0));
	EXPECT_EQ(4, square(-2));
	
}


TEST(CoordinateMapperTest, HandlesArrayInQuadrantI)
{
	CameraSpacePoint left = CameraSpacePoint();
	left.X = 2;
	left.Y = 0;
	left.Z = 3;

	CameraSpacePoint right = CameraSpacePoint();
	right.X = 3;
	right.Y = 0;
	right.Z = 4;

	CoordinateSystem grid = CoordinateSystem(left, right);
	CameraSpacePoint origin = grid.getOrigin();

	ASSERT_NEAR(2.5, origin.X, 0.1);
	ASSERT_NEAR(3.5, origin.Z, 0.1);

	CameraSpacePoint pt = CameraSpacePoint();
	pt.X = 0;
	pt.Y = 0;
	pt.Z = 3;

	double theta = grid.getOrientation();
	ASSERT_NEAR(0.785398, theta, ALLOWED_ERR);

	Coordinate loc = grid.cameraToSystemSpace(pt);


	ASSERT_NEAR(-2.1213203, loc.x, ALLOWED_ERR);
	ASSERT_NEAR(1.4142, loc.y, ALLOWED_ERR);
}

TEST(CoordinateMapperTest, HandlesArrayInQuadrantIII)
{
	CameraSpacePoint left = CameraSpacePoint();
	left.X = -2;
	left.Y = 0;
	left.Z = -1;

	CameraSpacePoint right = CameraSpacePoint();
	right.X = -1;
	right.Y = 0;
	right.Z = -2;

	CoordinateSystem grid = CoordinateSystem(left, right);
	CameraSpacePoint origin = grid.getOrigin();

	ASSERT_NEAR(-1.5, origin.X, 0.1);
	ASSERT_NEAR(-1.5, origin.Z, 0.1);

	CameraSpacePoint pt = CameraSpacePoint();
	pt.X = 2;
	pt.Y = 0;
	pt.Z = 2;

	double theta = grid.getOrientation();
	ASSERT_NEAR(5.497787, theta, ALLOWED_ERR);

	Coordinate loc = grid.cameraToSystemSpace(pt);


	ASSERT_NEAR(0, loc.x, ALLOWED_ERR);
	ASSERT_NEAR(4.949747, loc.y, ALLOWED_ERR);
}
