#include <stdint.h>
#include <string>
#include <iostream>
#include <Kinect.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include "coordinate-system.h"
#include "room.h"
#include "beamformer.h"

using namespace std;

//Define Room Thread variables

shared_mutex Room::target_mutex;
condition_variable_any Room::target_trigger;

//Constructor
Room::Room(CoordinateSystem xy) : grid(xy) {
	running = false;
}

Room::~Room() {
	cout << "Room deconstructor" << endl;

	if (running) {
		stop();
	}
	Shutdown();
}

void Room::Init()
{

	//put initialization stuff here
	HRESULT hr;
	hr = GetDefaultKinectSensor(&m_sensor);

	if (FAILED(hr))
	{
		printf("Failed to find the Kinect sensor!\n");
		exit(10);
	}

	m_sensor->Open();

	//Open BodyFrame Source
	IBodyFrameSource* bodyFrameSource;
	hr = m_sensor->get_BodyFrameSource(&bodyFrameSource);
	if (FAILED(hr))
	{
		printf("Failed to get body frame source.\n");
		exit(10);
	}

	hr = bodyFrameSource->OpenReader(&m_bodyFrameReader);
	if (FAILED(hr))
	{
		printf("Failed to open the Body Frame Reader.\n");
		exit(10);
	}
	SafeRelease(bodyFrameSource);
}

/**
*		Boolean predicate to determine if a body is untracked
*/
bool untracked(IBody* body) {
	BOOLEAN tracked; 
	HRESULT hr;
	hr = body->get_IsTracked(&tracked);
	if (FAILED(hr) || !(tracked)) {
		return true;
	}
	else {
		return false;
	}
}

void Room::processBodies(std::list<IBody*> &lBodies)
{
	//Lock down the targets so no one else accesses them
	std::unique_lock<std::shared_mutex> guard(Room::target_mutex);

	//Remove all bodies that are untracked
	lBodies.remove_if(untracked);


	map<UINT64, Target*>::iterator itTarget;
	list<IBody*>::iterator itBody;

	//Set all targets to untracked
	for (itTarget = m_targets.begin(); itTarget != m_targets.end(); itTarget++)
	{
		itTarget->second->setTracked(false);
	}

	UINT64 bodyId;
	Joint joints[JointType_Count];
	Joint headJoint;

	
	itBody = lBodies.begin();
	while (itBody != lBodies.end())
	{
		(*itBody)->get_TrackingId(&bodyId);

		//Check if the ID matches a body currently tracked in the room
		if ((itTarget = m_targets.find(bodyId)) != m_targets.end())
		{
			(*itBody)->GetJoints(JointType_Count, joints);
			headJoint = joints[JointType_Head];

			(itTarget)->second->setTracked(true);
			(itTarget)->second->updatePosition(grid.cameraToSystemSpace(headJoint.Position));

			//Updated target, can remove body (do not need to worry about deleting IBody object, that is done in caller method)
			itBody = lBodies.erase(itBody);
		}

		else {
			++itBody;
		}
	}

	//Delete all untracked targets
	itTarget = m_targets.begin();
	while (itTarget != m_targets.end())
	{
		if (itTarget->second->getTrackedStatus() == false)
		{
			delete itTarget->second;						//delete the target object
			m_targets.erase(itTarget++);	//remove the target pointer from the list
		}
		else {
			++itTarget;
		}
		
	}
	
	//Any remaining items in lBodies are newly tracked, and need to be added to m_targets
	for (itBody = lBodies.begin(); itBody != lBodies.end(); itBody++) {

		if (m_targets.size() < MAXBODIES)
		{
			(*itBody)->get_TrackingId(&bodyId);
			(*itBody)->GetJoints(JointType_Count, joints);
			headJoint = joints[JointType_Head];
			
			m_targets.insert(pair<UINT64, Target*>(bodyId, new Target(grid.cameraToSystemSpace(headJoint.Position), bodyId)));
		}
		else {
			printf("Error: Overran Target Buffer.\n");
		}
		
	}
}


map<UINT64, Target*>& Room::getTargetReference()
{
	return m_targets;
}


void Room::updateTargets()
{
	HRESULT hr;

	IBodyFrame *bodyFrame = nullptr;


	hr = m_bodyFrameReader->AcquireLatestFrame(&bodyFrame);
	if (SUCCEEDED(hr))
	{
		IBody* ppBodies[MAXBODIES] = { 0 };

		hr = bodyFrame->GetAndRefreshBodyData(MAXBODIES, ppBodies);

		if (SUCCEEDED(hr))
		{
			std::list<IBody*> lBodies;
			for (int i = 0; i < MAXBODIES; i++)
			{
				lBodies.push_back(ppBodies[i]);
			}

			processBodies(lBodies);
		}

		for (int i = 0; i < _countof(ppBodies); i++)
		{
			SafeRelease(ppBodies[i]);
		}

	}

	SafeRelease(bodyFrame);
	
}

void Room::DEBUG_GenerateTestTarget() {

	srand(time(NULL));

	Coordinate head( rand() % 20, rand() % 20);

	UINT64 id = rand();

	Target* targ = new Target(head, id);

	m_targets.insert(pair<UINT64, Target*>(id, targ));

}

void Room::DEBUG_ShuffleTargets() {

	std::map<UINT64, Target*>::iterator itr;

	srand(time(NULL));

	for (itr = m_targets.begin(); itr != m_targets.end(); itr++)
	{
		Coordinate head(rand() % 5, rand() % 5);

		itr->second->updatePosition(head);

		Sleep(50);
	}

}

void Room::waitForTargets() {

	shared_lock<shared_mutex> lck(Room::target_mutex);

	Room::target_trigger.wait(lck);

}

void Room::monitor(Beamformer* beamformer) {
	while (running) {
		Sleep(1000);
		this->updateTargets();
		target_trigger.notify_all();
		beamformer->updateTargets(this->getTargets());
	}
}

/**
 *	Updates whether or not a target is muted
 */
void Room::muteTargets(std::map<UINT64, BOOLEAN> mute_actions)
{
	unique_lock<shared_mutex> lck(target_mutex);

	std::map<UINT64, BOOLEAN>::iterator itr;
	std::map<UINT64, Target*>::iterator current;
	for (itr = mute_actions.begin(); itr != mute_actions.end(); itr++)
	{
		if ((current = m_targets.find(itr->first)) != m_targets.end()) {
			//If Target ID exists in map, update its mute value
			current->second->setMute(itr->second);
		}
	}
}

map<UINT64, Target*> Room::getTargets() {
	unique_lock<shared_mutex> lck(Room::target_mutex);
	return this->m_targets;
}

void Room::beginMonitoring(Beamformer* beamformer) {
	running = true;
	this->t_monitor = thread(&Room::monitor, this, beamformer);
}

void Room::stop() {
	running = false;
	if (t_monitor.joinable()) {
		t_monitor.join();
	}

	cout << "Room done" << endl;
}

void Room::Shutdown()
{
  //put cleaning up stuff here
	SafeRelease(m_bodyFrameReader);
	SafeRelease(m_sensor);
}
