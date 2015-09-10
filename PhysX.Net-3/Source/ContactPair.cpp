#include <StdAfx.h>
#include "ContactPair.h"
#include "Shape.h"
#include "Actor.h"

ContactPair^ ContactPair::ToManaged(const PxContactPair& pair)
{
	ContactPair^ managedPair = gcnew ContactPair();

	managedPair->Shapes = gcnew array<Shape^>(2);

	managedPair->Shapes[0] = ObjectTable::TryGetObject<Shape^>((intptr_t)pair.shapes[0]);
	managedPair->Shapes[1] = ObjectTable::TryGetObject<Shape^>((intptr_t)pair.shapes[1]);

	managedPair->ContactCount = pair.contactCount;
	managedPair->Flags = ToManagedEnum(ContactPairFlag, pair.flags);
	managedPair->Events = ToManagedEnum(PairFlag, pair.events);

	return managedPair;
}

ContactPairHeader^ ContactPairHeader::ToManaged(const PxContactPairHeader& unmanaged)
{
	ContactPairHeader^ managed = gcnew ContactPairHeader();

	managed->Actors = gcnew array<Actor^>(2);

	managed->Actors[0] = ObjectTable::TryGetObject<Actor^>((intptr_t)unmanaged.actors[0]);
	managed->Actors[1] = ObjectTable::TryGetObject<Actor^>((intptr_t)unmanaged.actors[1]);

	managed->Flags = ToManagedEnum(ContactPairHeaderFlag, unmanaged.flags);

	return managed;
}