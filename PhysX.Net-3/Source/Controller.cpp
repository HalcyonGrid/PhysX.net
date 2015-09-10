#include "StdAfx.h"
#include "Controller.h"
#include "ControllerManager.h"
#include "ControllerFilters.h"
#include "ObstacleContext.h"
#include "RigidDynamic.h"
#include "Scene.h"
#include "ObjectTable.h"
#include "Shape.h"

Controller::Controller(PxController* controller, PhysX::ControllerManager^ owner)
{
	if (controller == NULL)
		throw gcnew ArgumentNullException("controller");
	ThrowIfNullOrDisposed(owner, "owner");

	_controller = controller;
	_controllerManager = owner;

	ObjectTable::Add((intptr_t)controller, this, owner);
	_internalActor = gcnew RigidDynamic(controller->getActor(), owner->Scene->Physics);
	_internalActor->ConstructedByPhysx = true;

	PxShape** shapes = new PxShape*[1];
	PxU32 numReturned = controller->getActor()->getShapes(shapes, 1);

	if (numReturned == 1)
	{
		Shape^ actorShape = gcnew Shape(shapes[0], _internalActor, nullptr);
		_internalActor->TrackShape(actorShape);
	}

	delete[] shapes;

	
}



Controller::~Controller()
{
	this->!Controller();
	GC::SuppressFinalize(this);
}
Controller::!Controller()
{
	//ObjectTable::Remove(_internalActor);

	delete _internalActor;

	OnDisposing(this, nullptr);

	if (Disposed)
		return;

	_controller->release();
	_controller = NULL;

	UserData = nullptr;
	
	OnDisposed(this, nullptr);
}
bool Controller::Disposed::get()
{
	return (_controller == NULL);
}

class MoveCallback : public PxSceneQueryFilterCallback
{
public:
	virtual PxSceneQueryHitType::Enum preFilter(const PxFilterData& filterData0, PxShape* shape, PxSceneQueryFilterFlags& filterFlags)
	{
		PxFilterData filterData1 = shape->getQueryFilterData();
		if(! ((filterData0.word2 & filterData1.word3) && (filterData1.word2 & filterData0.word3)))
		{
			//objects don't agree to collide
			return PxSceneQueryHitType::eNONE;
		}

		return PxSceneQueryHitType::eBLOCK;
	}

	virtual PxSceneQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxSceneQueryHit& hit)
	{
		return PxSceneQueryHitType::eBLOCK;
	}

	virtual ~MoveCallback() {}
};

static MoveCallback* _controllerMoveCallback = new MoveCallback();

ControllerFlag Controller::Move(Vector3 displacement, TimeSpan elapsedTime)
{
	auto filter = gcnew ControllerFilters();
		filter->ActiveGroups = 0xFFFFFFFF;
		filter->FilterFlags = SceneQueryFilterFlag::Static | SceneQueryFilterFlag::Dynamic;

	return Move(displacement, elapsedTime, 0.001f, filter, nullptr);
}
ControllerFlag Controller::Move(Vector3 displacement, TimeSpan elapsedTime, float minimumDistance, ControllerFilters^ filters, [Optional] ObstacleContext^ obstacles)
{
	auto disp = MathUtil::Vector3ToPxVec3(displacement);
	auto et = (float)elapsedTime.TotalSeconds;
	auto f = ControllerFilters::ToUnmanaged(filters);
	f.mFilterCallback = _controllerMoveCallback;
	auto oc = (obstacles == nullptr ? NULL : obstacles->UnmanagedPointer);

	PxU32 retFlags = _controller->move(disp, minimumDistance, et, f, oc);

	// TODO: Not the cleanest way of cleaning up the memory that ControllerFilter allocates
	if (f.mFilterData != NULL)
	{
		delete f.mFilterData;
	}

	return (ControllerFlag)retFlags;
}

void Controller::InvalidateCache()
{
	_controller->invalidateCache();
}

PhysX::ControllerManager^ Controller::ControllerManager::get()
{
	return _controllerManager;
}

Vector3 Controller::Position::get()
{
	PxExtendedVec3 p = _controller->getPosition();

	return MathUtil::PxExtendedVec3ToVector3(p);
}
void Controller::Position::set(Vector3 value)
{
	PxExtendedVec3 p = MathUtil::Vector3ToPxExtendedVec3(value);

	_controller->setPosition(p);
}

float Controller::StepOffset::get()
{
	return _controller->getStepOffset();
}
void Controller::StepOffset::set(float value)
{
	_controller->setStepOffset(value);
}

CCTInteractionMode Controller::Interaction::get()
{
	return ToManagedEnum(CCTInteractionMode, _controller->getInteraction());
}
void Controller::Interaction::set(CCTInteractionMode value)
{
	_controller->setInteraction(ToUnmanagedEnum(PxCCTInteractionMode, value));
}

float Controller::ContactOffset::get()
{
	return _controller->getContactOffset();
}

Vector3 Controller::UpDirection::get()
{
	return MathUtil::PxVec3ToVector3(_controller->getUpDirection());
}
void Controller::UpDirection::set(Vector3 value)
{
	_controller->setUpDirection(MathUtil::Vector3ToPxVec3(value));
}

float Controller::SlopeLimit::get()
{
	return _controller->getSlopeLimit();
}

ControllerShapeType Controller::Type::get()
{
	return ToManagedEnum(ControllerShapeType, _controller->getType());
}

PxController* Controller::UnmanagedPointer::get()
{
	return _controller;
}

RigidDynamic^ Controller::Actor::get()
{
	return _internalActor;
}

void Controller::ShapeFilterData::set(FilterData filterData)
{
	PxRigidActor* actor = _controller->getActor();

	PxShape** shapes = new PxShape*[1];
	PxU32 numReturned = actor->getShapes(shapes, 1);

	if (numReturned == 1)
	{
		PxFilterData nativeData(FilterData::ToUnmanaged(filterData));
		shapes[0]->setSimulationFilterData(nativeData);
		shapes[0]->setQueryFilterData(nativeData);
	}

	delete[] shapes;
}