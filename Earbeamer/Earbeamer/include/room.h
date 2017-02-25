#pragma once

//some often used STL header files
#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <string>
#include <Kinect.h>
#include <map>
#include <utility>	//use for pair type definition

#include "target.h"
#include "coordinate-system.h"

#define MAXBODIES 6

using namespace std;

//safe way of deleting a COM object
template<typename T>
void SafeRelease(T& ptr) { if(ptr) { ptr->Release(); ptr = nullptr; } }

class Room
{
public:
	Room(CoordinateSystem xy) : grid(xy) {};
	void Init();
	void Shutdown();
	void getTargets(map<UINT64, Target*> &targs);
	void updateTargets();

private:
	IKinectSensor* m_sensor = nullptr;
	IBodyFrameReader* m_bodyFrameReader = nullptr;
	std::map<UINT64, Target*> m_targets;
	void processBodies(std::list<IBody*> &vBodies);
	

protected:
	CoordinateSystem grid;
  
};
