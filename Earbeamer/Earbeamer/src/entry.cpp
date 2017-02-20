#include <iostream>
#include <windows.h>
#include <map>

#include "room.h"
#include "coordinate-system.h"


using namespace std;



int main(int argc, char *argv[]) {
	CameraSpacePoint left = CameraSpacePoint();
	left.X = 1.0;
	left.Y = 0;
	left.Z = 1.0;

	CameraSpacePoint right = CameraSpacePoint();
	right.X = -1.0;
	right.Y = 0;
	right.Z = 1.0;

	CoordinateSystem grid = CoordinateSystem(left, right);

	Room room = Room(grid);

	room.Init();

	map<UINT64, Target*> targs;
	map<UINT64, Target*>::iterator itr;
	cout << "Here we go" << endl;
	while (true)
	{
		Sleep(1000);
		room.updateTargets();
		room.getTargets(targs);

		for (itr = targs.begin(); itr != targs.end(); itr++)
		{
			Coordinate coord = itr->second->getPosition();
			cout << "ID: " << itr->second->getTrackingId();
			cout << "Coordinate: " << coord << endl;
			cout << "Angle: " << itr->second->getAngleFromOrigin() << endl;
		}

	}


}