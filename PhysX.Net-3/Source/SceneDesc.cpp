#include "StdAfx.h"
#include "SceneDesc.h"
#include "TolerancesScale.h"
#include "Physics.h"
#include <PxSceneDesc.h>
#include <PxDefaultSimulationFilterShader.h>
#include <PxDefaultCpuDispatcher.h>
#include <PxCpuDispatcher.h>

#include <iostream>

using namespace PhysX;
using namespace PhysX::Math;

PxFilterFlags CCDFilterShader(
        PxFilterObjectAttributes attributes0,
        PxFilterData filterData0,
        PxFilterObjectAttributes attributes1,
        PxFilterData filterData1,
        PxPairFlags& pairFlags,
        const void* constantBlock,
        PxU32 constantBlockSize)
{
        pairFlags = PxPairFlag::eCONTACT_DEFAULT;
		pairFlags |= (PxPairFlags) filterData0.word0 | (PxPairFlags) filterData1.word0;

		if (filterData0.word1 == 1 || filterData1.word1 == 1)
		{
			//kill all contacts except the ground
			if (filterData0.word2 != (1 << 2) && filterData1.word2 != (1 << 2))
			{
				pairFlags &= ~PxPairFlag::eRESOLVE_CONTACTS;
				return PxFilterFlag::eKILL;
			}
		}
		else if(! ((filterData0.word2 & filterData1.word3) && (filterData1.word2 & filterData0.word3)))
		{
			//objects don't agree to collide, disable contact resolving and don't enable ccd 
			//(ccd casues collision artifacts)
			pairFlags &= ~PxPairFlag::eRESOLVE_CONTACTS;
		}
		else
		{
			//enable ccd unless the pair has turned it off
			if (filterData0.word1 != 2 && filterData1.word1 != 2)
			{
				pairFlags |= PxPairFlag::eSWEPT_INTEGRATION_LINEAR;
			}
		}

		

		return PxFilterFlags();
}

PxFilterFlags StandardFilterShader(
        PxFilterObjectAttributes attributes0,
        PxFilterData filterData0,
        PxFilterObjectAttributes attributes1,
        PxFilterData filterData1,
        PxPairFlags& pairFlags,
        const void* constantBlock,
        PxU32 constantBlockSize)
{
        pairFlags = PxPairFlag::eCONTACT_DEFAULT;
		pairFlags |= (PxPairFlags) filterData0.word0 | (PxPairFlags) filterData1.word0;

		if (filterData0.word1 == 1 || filterData1.word1 == 1)
		{
			//kill all contacts except the ground
			if (filterData0.word2 != (1 << 2) && filterData1.word2 != (1 << 2))
			{
				pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
				return PxFilterFlag::eKILL | PxFilterFlag::eCALLBACK;
			}
		}
		else if(! ((filterData0.word2 & filterData1.word3) && (filterData1.word2 & filterData0.word3)))
		{
			//objects don't agree to collide
			pairFlags &= ~PxPairFlag::eRESOLVE_CONTACTS;
		}

        return PxFilterFlags();
}


class DefaultSimulationFilterCallback : public PxSimulationFilterCallback
{
	virtual PxFilterFlags pairFound(PxU32 pairID, PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxActor *a0, PxShape *s0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxActor *a1, PxShape *s1, PxPairFlags &pairFlags)
	{
		if (filterData0.word1 == 1 || filterData1.word1 == 1)
		{
			//kill all contacts except the ground
			if (filterData0.word2 != (1 << 2) && filterData1.word2 != (1 << 2))
			{
				pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
				return PxFilterFlag::eKILL;
			}
		}

		return PxFilterFlag::eDEFAULT;
	}
 
	virtual void pairLost(PxU32 pairID, PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, bool objectDeleted)
	{
		
	}
 
	virtual bool statusChange(PxU32 &pairID, PxPairFlags &pairFlags, PxFilterFlags &filterFlags)
	{
		return false;
	}

};

DefaultSimulationFilterCallback* _defaultSimulationFilterCallback = new DefaultSimulationFilterCallback();

SceneDesc::SceneDesc([Optional] Nullable<PhysX::TolerancesScale> tolerancesScale)
{
	if (!Physics::Instantiated)
		throw gcnew InvalidOperationException("To create a SceneDesc instance you first must create a Physics instance. Internally this allows PxInitExtensions to be called.");

	PxTolerancesScale ts = tolerancesScale.HasValue ? TolerancesScale::ToUnmanaged(tolerancesScale.Value) : PxTolerancesScale();

	_sceneDesc = new PxSceneDesc(ts);
	_sceneDesc->filterShader = StandardFilterShader;
	_sceneDesc->filterCallback = _defaultSimulationFilterCallback;

	_sceneDesc->cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
}
 
SceneDesc::SceneDesc(Nullable<PhysX::TolerancesScale> tolerancesScale, bool useCcdFilterShader)
{
	if (!Physics::Instantiated)
		throw gcnew InvalidOperationException("To create a SceneDesc instance you first must create a Physics instance. Internally this allows PxInitExtensions to be called.");

	PxTolerancesScale ts = tolerancesScale.HasValue ? TolerancesScale::ToUnmanaged(tolerancesScale.Value) : PxTolerancesScale();

	_sceneDesc = new PxSceneDesc(ts);

	if (useCcdFilterShader)
	{
		_sceneDesc->filterShader = CCDFilterShader;
	}
	else
	{
		_sceneDesc->filterShader = StandardFilterShader;
	}

	_sceneDesc->filterCallback = _defaultSimulationFilterCallback;

	_sceneDesc->cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
}
SceneDesc::~SceneDesc()
{
	this->!SceneDesc();
}
SceneDesc::!SceneDesc()
{
	SAFE_DELETE(_sceneDesc);
}
bool SceneDesc::Disposed::get()
{
	return _sceneDesc == NULL;
}

bool SceneDesc::IsValid()
{
	return _sceneDesc->isValid();
}
void SceneDesc::SetToDefault([Optional] Nullable<PhysX::TolerancesScale> tolerancesScale)
{
	if (tolerancesScale.HasValue)
		_sceneDesc->setToDefault(PhysX::TolerancesScale::ToUnmanaged(tolerancesScale.Value));
	else
		_sceneDesc->setToDefault(PxTolerancesScale());
}

Vector3 SceneDesc::Gravity::get()
{
	return MathUtil::PxVec3ToVector3(_sceneDesc->gravity);
}
void SceneDesc::Gravity::set(Vector3 value)
{
	_sceneDesc->gravity = MathUtil::Vector3ToPxVec3(value);
}

PhysX::SimulationEventCallback^ SceneDesc::SimulationEventCallback::get()
{
	return _simulationEventCallback;
}
void SceneDesc::SimulationEventCallback::set(PhysX::SimulationEventCallback^ value)
{
	_simulationEventCallback = value;

	_sceneDesc->simulationEventCallback = (value == nullptr ? NULL : value->UnmanagedPointer);
}

PxSceneDesc* SceneDesc::UnmanagedPointer::get()
{
	return _sceneDesc;
}