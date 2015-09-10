#pragma once

#include <PxSimulationEventCallback.h> 
#include "PhysicsEnum.h"

namespace PhysX
{
	ref class Actor;
	ref class Shape;

	///<summary>
	/// Collection of flags providing information on contact report pairs
	///</summary>
	public enum class ContactPairHeaderFlag
	{
		DeletedActor0 = PxContactPairHeaderFlag::eDELETED_ACTOR_0,
		DeletedActor1 = PxContactPairHeaderFlag::eDELETED_ACTOR_1
	};

	///<summary>
	/// An Instance of this class is passed to PxSimulationEventCallback.onContact(). 
	///</summary>
	public ref class ContactPairHeader
	{
		public:
			property array<Actor^>^ Actors;
			property ContactPairHeaderFlag Flags;

		internal:
			static ContactPairHeader^ ToManaged(const PxContactPairHeader& unmanaged);
	};

	///<summary>
	/// Contact report pair information. 
	/// Instances of this class are passed to PxSimulationEventCallback.onContact(). If contact reports have been requested for a pair of shapes (see PxPairFlag), then the corresponding contact information will be provided through this structure.
	///</summary>
	public ref class ContactPair
	{
		public:
			property array<Shape^>^ Shapes;
			property unsigned short ContactCount;
			property ContactPairFlag Flags;
			property PairFlag Events;

		internal:
			static ContactPair^ ToManaged(const PxContactPair& pair);
	};
}