#include "StdAfx.h"
#include "Actor.h"
#include "Physics.h"
#include "ObservableInstance.h"
#include "Serializable.h"
#include "RigidActor.h"
#include "Shape.h"

using namespace PhysX;

Actor::Actor(PxActor* actor, PhysX::Physics^ owner)
{
	if (actor == NULL)
		throw gcnew ArgumentNullException("actor");
	ThrowIfNullOrDisposed(owner, "owner");

	_actor = actor;
	_physics = owner;
	_disposing = false;
	_constructedByPhysx = false;

	ObjectTable::Add((intptr_t)actor, this, owner);
}
Actor::~Actor()
{
	this->!Actor();
	GC::SuppressFinalize(this);
}
Actor::!Actor()
{
	_disposing = true;
	if (_constructedByPhysx)
	{
		//manually remove our managed shapes
		RigidActor^ thisRigid = dynamic_cast<RigidActor^>(this);

		if (thisRigid != nullptr)
		{
			for each (Shape^ shape in thisRigid->Shapes)
			{
				delete shape;
			}
		}

		ObjectTable::Remove(this);
		return;
	}

	OnDisposing(this, nullptr);

	if (Disposed)
		return;

	_actor->release();
	_actor = NULL;

	OnDisposed(this, nullptr);
}

bool Actor::Disposed::get()
{
	return (_actor == NULL);
}

bool Actor::Disposing::get()
{
	return _disposing;
}

void Actor::ConstructedByPhysx::set(bool value)
{
	_constructedByPhysx = value;
}

ObservableInstance^ Actor::GetObservableInstance()
{
	return gcnew ObservableInstance(_actor);
}

Serializable^ Actor::AsSerializable()
{
	return gcnew Serializable(_actor);
}

String^ Actor::ToString()
{
	return this->Name;
}

//

PhysX::Physics^ Actor::Physics::get()
{
	return _physics;
}

ActorType Actor::Type::get()
{
	return ToManagedEnum(ActorType, _actor->getType());
}

String^ Actor::Name::get()
{
	return Util::ToManagedString(_actor->getName());
}
void Actor::Name::set(String^ value)
{
	if (_actor->getName() != NULL)
		Marshal::FreeHGlobal(IntPtr((char*)_actor->getName()));

	_actor->setName(Util::ToUnmanagedString(value));
}

Bounds3 Actor::WorldBounds::get()
{
	return Bounds3::FromUnmanaged(_actor->getWorldBounds());
}

ActorFlag Actor::Flags::get()
{
	return ToManagedEnum(ActorFlag, _actor->getActorFlags());
}
void Actor::Flags::set(ActorFlag value)
{
	_actor->setActorFlags((PxActorFlags)(PxU32)value);
}

PxActor* Actor::UnmanagedPointer::get()
{
	return _actor;
}