#include <stdint.h>
#include <string>
#include <iostream>
#include <Kinect.h>

#include "coordinate-system.h"
#include "room.h"

using namespace std;

//Constructor

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
	cout << "Initializing" << endl;
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
	//Remove all bodies that are untracked
	lBodies.remove_if(untracked);

	//Set all targets to untracked
	list<Target*>::iterator itTarget;
	list<IBody*>::iterator itBody;

	for (itTarget = m_targets.begin(); itTarget != m_targets.end(); itTarget++)
	{
		(*itTarget)->setTracked(false);
	}

	UINT64 bodyId;
	Joint joints[JointType_Count];
	Joint headJoint;
	if (lBodies.size() > 0) {
		cout << "Found one" << endl;
	}
	
	itBody = lBodies.begin();
	while (itBody != lBodies.end())
	{
		(*itBody)->get_TrackingId(&bodyId);
		(*itBody)->GetJoints(JointType_Count, joints);
		headJoint = joints[JointType_Head];

		//Look for a target with the same tracking id, update
		for (itTarget = m_targets.begin(); itTarget != m_targets.end(); itTarget++)
		{
			if ((*itTarget)->getTrackingId() == bodyId)
			{
				(*itTarget)->setTracked(true);
				(*itTarget)->updatePosition(grid.cameraToSystemSpace(headJoint.Position));

				//Updated target, can remove body (do not need to worry about deleting IBody object, that is done in caller method)
				lBodies.erase(itBody);

				break;
			}
		}
	}
	/*
	for (itBody = lBodies.begin(); itBody != lBodies.end(); itBody++)
	{
		(*itBody)->get_TrackingId(&bodyId);
		(*itBody)->GetJoints(JointType_Count, joints);
		headJoint = joints[JointType_Head];

		//Look for a target with the same tracking id, update
		for (itTarget = m_targets.begin(); itTarget != m_targets.end(); itTarget++)
		{
			if ((*itTarget)->getTrackingId() == bodyId)
			{
				(*itTarget)->setTracked(true);
				(*itTarget)->updatePosition(grid.cameraToSystemSpace(headJoint.Position));

				//Updated target, can remove body (do not need to worry about deleting IBody object, that is done in caller method)
				lBodies.erase(itBody);

				break;
			}
		}
		
	}
	*/
	/*
	//Delete all untracked targets
	for (itTarget = m_targets.begin(); itTarget != m_targets.end(); itTarget++) {
		if ((*itTarget)->getTrackedStatus() == false)
		{
			delete *itTarget;			//delete the target object
			m_targets.erase(itTarget);	//remove the target pointer from the list
		}
	}
	*/
	
	//Delete all untracked targets
	itTarget = m_targets.begin();
	while (itTarget != m_targets.end())
	{
		if ((*itTarget)->getTrackedStatus() == false)
		{
			delete *itTarget;						//delete the target object
			itTarget = m_targets.erase(itTarget);	//remove the target pointer from the list
		}
		else {
			++itTarget;
		}
	}
	

	//Any remaining items in lBodies are newly tracked, and need to be added to m_targets
	for (itBody = lBodies.begin(); itBody != lBodies.end(); itBody++) {
		
		if (m_targets.size() < MAXBODIES)
		{
			m_targets.push_back(new Target(grid.cameraToSystemSpace(headJoint.Position), bodyId));
		}
		else {
			printf("Error: Overran Target Buffer.\n");
		}
		
	}
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

void Room::getTargets(list<Target*> &targs) {
	targs = m_targets;
}


void Room::Shutdown()
{
  //put cleaning up stuff here
	SafeRelease(m_bodyFrameReader);
	SafeRelease(m_sensor);
}
