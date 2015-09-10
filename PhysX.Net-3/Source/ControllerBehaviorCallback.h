#pragma once

#include "ControllerEnum.h"
#include "Shape.h"
#include "Controller.h"

namespace PhysX
{
	ref class ControllerBehaviorCallback;

	class InternalControllerBehaviorCallback : public PxControllerBehaviorCallback
	{
		private:
			gcroot<ControllerBehaviorCallback^> _behaviorCallback;

		public:
			InternalControllerBehaviorCallback(ControllerBehaviorCallback^ hitReport);

			PxU32 getBehaviorFlags(const PxObstacle& hit);
			PxU32 getBehaviorFlags(const PxController& hit);
			PxU32 getBehaviorFlags(const PxShape& hit);
	};

	public ref class ControllerBehaviorCallback abstract
	{
	private:
		InternalControllerBehaviorCallback* _behaviorCallback;

	protected:
		ControllerBehaviorCallback();
		~ControllerBehaviorCallback();
	public:
		!ControllerBehaviorCallback();

		/// <summary>
		/// Retrieve behavior flags for a shape.
		/// When the CCT touches a shape, the CCT's behavior w.r.t. this shape can be customized
		/// by users. This function retrives the desired ControllerBehaviorFlag flags capturing
		/// the desired behavior.
		/// </summary>
		virtual ControllerBehaviorFlag GetBehaviorFlags(Shape^ shape) abstract;

		/// <summary>
		/// Retrieve behavior flags for a controller.
		/// When the CCT touches a controller, the CCT's behavior w.r.t. this controller can be customized
		/// by users. This function retrives the desired PxControllerBehaviorFlag flags capturing
		/// the desired behavior.
		/// </summary>
		virtual ControllerBehaviorFlag GetBehaviorFlags(Controller^ controller) abstract;

		/// <summary>
		/// 
		/// </summary>
		//ControllerBehaviorFlag GetBehaviorFlags(Obstacle^ obstacle);
	
	internal:
		property PxControllerBehaviorCallback* UnmanagedPointer
		{
			PxControllerBehaviorCallback* get();
		}
	};
};