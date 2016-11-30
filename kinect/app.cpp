#include "app.h"
#include <stdint.h>
#include <string>

using namespace std;

//Constructor
App::App(){}

void App::Init()
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

	IDepthFrameSource* depthFrameSource;
	hr = m_sensor->get_DepthFrameSource(&depthFrameSource);
	if (FAILED(hr))
	{
		printf("Failed to get the depth frame source.\n");
		exit(10);
	}

	hr = depthFrameSource->OpenReader(&m_depthFrameReader);
	if (FAILED(hr))
	{
		printf("Failed to open the depth frame reader.\n");
		exit(10);
	}

	SafeRelease(depthFrameSource);

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

	m_depthBuffer = new uint16[512 * 424];
}

/**
*	Retrieves depth data from frame, copies to pixel buffer
*
*/

void App::getDepthData()
{
	HRESULT hr;

	IDepthFrame* depthFrame = nullptr;

	hr = m_depthFrameReader->AcquireLatestFrame(&depthFrame);

	if (SUCCEEDED(hr))
	{
		depthFrame->CopyFrameDataToArray(512 * 424, m_depthBuffer);

		//copy to pixel buffer
		for (int i = 0; i < SCRWIDTH * SCRHEIGHT; i++)
		{
			uint16_t d = m_depthBuffer[i];

			UINT8 a = d & 0xff;

			m_pixelBuffer[i] = (a << 16) | (a << 8) | a;
		}
	}

	
	SafeRelease(depthFrame);

	
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



void App::processBodies(std::list<IBody*> &lBodies)
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
				(*itTarget)->updatePosition(headJoint.Position);

				//Updated target, can remove body (do not need to worry about deleting IBody object, that is done in caller method)
				lBodies.erase(itBody);

				break;
			}
		}
		
	}
	//Delete all untracked targets
	for (itTarget = m_targets.begin(); itTarget != m_targets.end(); itTarget++) {
		if ((*itTarget)->getTrackedStatus() == false)
		{
			delete *itTarget;			//delete the target object
			m_targets.erase(itTarget);	//remove the target pointer from the list
		}
	}


	//Any remaining items in lBodies are newly tracked, and need to be added to m_targets
	for (itBody = lBodies.begin(); itBody != lBodies.end(); itBody++) {
		
		if (m_targets.size() < MAXBODIES)
		{
			m_targets.push_back(new Target(headJoint.Position, bodyId));
		}
		else {
			printf("Error: Overran Target Buffer.\n");
		}
		
	}
}




void App::updateTargets()
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

void App::getTargets(list<Target*> &targs) {
	targs = m_targets;
}



/**
*	Represents one cycle of app
*	Get new depth pixels to display, as well as update the positions of all tracked targets
*/
void App::Tick()
{

	getDepthData();

	updateTargets();
	
	
}

void App::Shutdown()
{
  //put cleaning up stuff here
	delete[] m_depthBuffer;
	SafeRelease(m_bodyFrameReader);
	SafeRelease(m_depthFrameReader);
	SafeRelease(m_sensor);
}
