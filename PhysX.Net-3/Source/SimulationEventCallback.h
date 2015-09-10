#pragma once

#include <PxSimulationEventCallback.h>

namespace PhysX
{
	ref class SimulationEventCallback;
	ref class TriggerPair;
	ref class Actor;
	ref class ContactPairHeader;
	ref class ContactPair;

	class InternalSimulationEventCallback : public PxSimulationEventCallback
	{
	private:
		gcroot<SimulationEventCallback^> _eventCallback;

	public:
		virtual void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count);
		virtual void onWake(PxActor **actors, PxU32 count);
		virtual void onSleep(PxActor **actors, PxU32 count);
		virtual void onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs);
		virtual void onTrigger(PxTriggerPair *pairs, PxU32 count);

		InternalSimulationEventCallback(SimulationEventCallback^ callback);
		virtual ~InternalSimulationEventCallback();
	};

	/// <summary>
	/// An interface class that the user can implement in order to receive simulation events
	/// </summary>
	/// <remarks>
	/// Create an instance of a class that inherits from this one and register with the SceneDesc
	/// to receieve notifications of simulation events
	/// </remarks>
	public ref class SimulationEventCallback abstract : IDisposable
	{
		public:
			/// <summary>Raised before any disposing is performed.</summary>
			virtual event EventHandler^ OnDisposing;
			/// <summary>Raised once all disposing is performed.</summary>
			virtual event EventHandler^ OnDisposed;

			property bool Disposed
			{
				virtual bool get();
			}

		private:
			InternalSimulationEventCallback* _eventCallback;

		protected:
			SimulationEventCallback();
		public:
			~SimulationEventCallback();
		protected:
			!SimulationEventCallback();

		internal:
			property InternalSimulationEventCallback* UnmanagedPointer
			{
				InternalSimulationEventCallback* get();
			}

	public:
		//virtual void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) abstract;
		virtual void OnWake(array<Actor^>^ actors) abstract;
		virtual void OnSleep(array<Actor^>^ actors) abstract;
		virtual void OnContact(ContactPairHeader^ contactPairHeader, array<ContactPair^>^ pairs) abstract;
		virtual void OnTrigger(array<TriggerPair^>^ pairs) abstract;
	};
}