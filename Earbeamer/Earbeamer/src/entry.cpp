#include <iostream>
#include <windows.h>
#include <map>

#include "room.h"
#include "coordinate-system.h"
#include "websocket_server.hpp"


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

	Room* room = new Room(grid);

	room->Init();

	map<UINT64, Target*> targs;
	map<UINT64, Target*>::iterator itr;
	cout << "Here we go" << endl;
	
	Target test = Target(Coordinate(0, 0), 64);
	cout << "Target" << test << endl;


	WebsocketServer* server = new WebsocketServer(*room);

	room->beginMonitoring();
	server->run(9002);
	server->begin_broadcast();

	while (1)
	{

	}


	/*
	while (true)
	{
		Sleep(1000);
		room.updateTargets();
		room.getTargets(targs);

	

		for (itr = targs.begin(); itr != targs.end(); itr++)
		{
			cout << "Testing JSON: " << endl;
			cout << *(itr->second) << endl;
			Coordinate coord = itr->second->getPosition();
			cout << "ID: " << itr->second->getTrackingId();
			cout << "Coordinate: " << coord << endl;
			cout << "Angle: " << itr->second->getAngleFromOrigin() << endl;
		}

	}
	*/


}