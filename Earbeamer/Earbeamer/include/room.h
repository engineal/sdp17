#pragma once

//some often used STL header files
#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <string>
#include <Kinect.h>
#include <map>
#include <utility>
#include <shared_mutex>
#include <condition_variable>

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
	static std::shared_mutex target_mutex;				//Used to control access to targets
	static std::condition_variable_any target_trigger;	//Used to notify when targets have been updated

	Room(CoordinateSystem xy) : grid(xy) {};
	~Room();
	void Init();
	void Shutdown();
	map<UINT64, Target*> getTargets();
	map<UINT64, Target*>& getTargetReference();
	void updateTargets();
	void beginMonitoring();

private:
	thread t_monitor;
	IKinectSensor* m_sensor = nullptr;
	IBodyFrameReader* m_bodyFrameReader = nullptr;
	std::map<UINT64, Target*> m_targets;
	void processBodies(std::list<IBody*> &vBodies);
	void monitor();
	

protected:
	CoordinateSystem grid;
  
};
