#include "StdAfx.h"
#include "SimulationEventCallback.h"
#include "TriggerPair.h"
#include "Actor.h"
#include "ContactPair.h"
#include <iostream>

InternalSimulationEventCallback::InternalSimulationEventCallback(SimulationEventCallback^ callback)
	: PxSimulationEventCallback()
{
	ThrowIfNull(callback, "callback");

	_eventCallback = callback;
}

InternalSimulationEventCallback::~InternalSimulationEventCallback()
{
}

void InternalSimulationEventCallback::onConstraintBreak(PxConstraintInfo *constraints, PxU32 count)
{
	//TODO: Unimplemented
}

void InternalSimulationEventCallback::onWake(PxActor **actors, PxU32 count)
{
	array<Actor^>^ actorList = gcnew array<Actor^>(count);

	for (int i = 0; i < count; i++)
	{
		actorList[i] = ObjectTable::GetObject<Actor^>((intptr_t)actors[i]);
	}

	_eventCallback->OnWake(actorList);
}

void InternalSimulationEventCallback::onSleep(PxActor **actors, PxU32 count)
{
	array<Actor^>^ actorList = gcnew array<Actor^>(count);

	for (PxU32 i = 0; i < count; i++)
	{
		actorList[i] = ObjectTable::GetObject<Actor^>((intptr_t)actors[i]);
	}

	_eventCallback->OnSleep(actorList);
}

void InternalSimulationEventCallback::onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs)
{
	ContactPairHeader^ managedHeader = ContactPairHeader::ToManaged(pairHeader);

	array<ContactPair^>^ managedPairs = gcnew array<ContactPair^>(nbPairs);
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		managedPairs[i] = ContactPair::ToManaged(pairs[i]);
	}

	_eventCallback->OnContact(managedHeader, managedPairs);
}

void InternalSimulationEventCallback::onTrigger(PxTriggerPair *pairs, PxU32 count)
{
	array<TriggerPair^>^ pairList = gcnew array<TriggerPair^>(count);

	for (PxU32 i = 0; i < count; i++)
	{
		pairList[i] = TriggerPair::ToManaged(pairs[i]);
	}

	_eventCallback->OnTrigger(pairList);
}

SimulationEventCallback::SimulationEventCallback()
{
	_eventCallback = new InternalSimulationEventCallback(this);
}

SimulationEventCallback::~SimulationEventCallback()
{
	this->!SimulationEventCallback();
}

SimulationEventCallback::!SimulationEventCallback()
{
	SAFE_DELETE(_eventCallback);
}

bool SimulationEventCallback::Disposed::get()
{
	return _eventCallback == NULL;
}

InternalSimulationEventCallback* SimulationEventCallback::UnmanagedPointer::get()
{
	return _eventCallback;
}