#pragma once

//some often used C standard library header files
#include <cstdlib>
#include <cstdio>
#include <cstring>

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
//some useful typedefs for explicit type sizes
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

//safe way of deleting a COM object
template<typename T>
void SafeRelease(T& ptr) { if(ptr) { ptr->Release(); ptr = nullptr; } }

class Room
{
public:
	Room(CoordinateSystem xy) : grid(xy) {};
	void Init();
	void Shutdown();
	void getTargets(list<Target*> &targs);
	void updateTargets();

private:
	IKinectSensor* m_sensor = nullptr;
	IBodyFrameReader* m_bodyFrameReader = nullptr;
	std::list<Target*> l_targets;
	std::map<UINT64, Target*> m_targets;
	void processBodies(std::list<IBody*> &vBodies);
	

protected:
	CoordinateSystem grid;
  
};