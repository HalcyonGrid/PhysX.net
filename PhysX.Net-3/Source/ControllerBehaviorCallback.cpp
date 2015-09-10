#include "StdAfx.h"
#include "ControllerBehaviorCallback.h"

InternalControllerBehaviorCallback::InternalControllerBehaviorCallback(ControllerBehaviorCallback^ behaviorCallback)
	: PxControllerBehaviorCallback()
{
	ThrowIfNull(behaviorCallback, "behaviorCallback");

	_behaviorCallback = behaviorCallback;
}
PxU32 InternalControllerBehaviorCallback::getBehaviorFlags(const PxObstacle& obstacle)
{
	return 0;
}

PxU32 InternalControllerBehaviorCallback::getBehaviorFlags(const PxController& controller)
{
	return 0;
}

PxU32 InternalControllerBehaviorCallback::getBehaviorFlags(const PxShape& shape)
{
	auto managedShape = ObjectTable::TryGetObject<PhysX::Shape^>((intptr_t)&shape);

	if (managedShape == nullptr)
	{
		return 0;
	}
	else
	{
		return (PxU32)_behaviorCallback->GetBehaviorFlags(managedShape);
	}
}

ControllerBehaviorCallback::ControllerBehaviorCallback()
{
	_behaviorCallback = new InternalControllerBehaviorCallback(this);
}
ControllerBehaviorCallback::~ControllerBehaviorCallback()
{
	this->!ControllerBehaviorCallback();
}
ControllerBehaviorCallback::!ControllerBehaviorCallback()
{
	SAFE_DELETE(_behaviorCallback);
}

PxControllerBehaviorCallback* ControllerBehaviorCallback::UnmanagedPointer::get()
{
	return _behaviorCallback;
}