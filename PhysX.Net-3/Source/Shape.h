#pragma once

#include "GeometryEnum.h"
#include "Bounds3.h"
#include "ShapeFlag.h"
#include <PxShape.h>

namespace PhysX
{
	ref class Actor;
	ref class Geometry;
	ref class Serializable;
	ref class BoxGeometry;
	ref class SphereGeometry;
	ref class CapsuleGeometry;
	ref class PlaneGeometry;
	ref class ConvexMeshGeometry;
	ref class TriangleMeshGeometry;
	ref class HeightFieldGeometry;
	value class FilterData;
	ref class Material;

	/// <summary>
	/// Abstract class for collision shapes.
	/// An instance can be created by calling the CreateShape() method of the RigidActor class, or
	/// by adding the shape descriptors into the RigidActorDesc class before creating the actor.
	/// </summary>
	public ref class Shape : IDisposable
	{
		public:
			/// <summary>Raised before any disposing is performed.</summary>
			virtual event EventHandler^ OnDisposing;
			/// <summary>Raised once all disposing is performed.</summary>
			virtual event EventHandler^ OnDisposed;

		private:
			PxShape* _shape;
			Actor^ _actor;
			LinkedListNode<Shape^>^ _parentLink;
			Geometry^ _geometry;

		internal:
			Shape(PxShape* shape, PhysX::Actor^ parentActor, PhysX::Geometry^ geom);
		public:
			~Shape();
		protected:
			!Shape();

		public:
			property bool Disposed
			{
				virtual bool get();
			}

			/// <summary>
			/// Gets an object which is responsible for serialization of this type.
			/// </summary>
			Serializable^ AsSerializable();

			BoxGeometry^ GetBoxGeometry();
			SphereGeometry^ GetSphereGeometry();
			CapsuleGeometry^ GetCapsuleGeometry();
			PlaneGeometry^ GetPlaneGeometry();
			ConvexMeshGeometry^ GetConvexMeshGeometry();
			TriangleMeshGeometry^ GetTriangleMeshGeometry();
			HeightFieldGeometry^ GetHeightFieldGeometry();

			//

			/// <summary>
			/// Get the geometry type of the shape.
			/// </summary>
			property PhysX::GeometryType GeometryType
			{
				PhysX::GeometryType get();
			}

			/// <summary>
			/// Retrieves the actor which this shape is associated with. 
			/// </summary>
			property PhysX::Actor^ Actor
			{
				PhysX::Actor^ get();
			}

			/// <summary>
			/// Retrieves the world space pose of the shape.
			/// </summary>
			property Matrix GlobalPose
			{
				Matrix get();
			}

			/// <summary>
			/// Gets or set the name of the shape.
			/// </summary>
			property String^ Name
			{
				String^ get();
				void set(String^ value);
			}

			/// <summary>
			/// Gets or sets the axis aligned bounding box enclosing the shape.
			/// </summary>
			property Bounds3 WorldBounds
			{
				Bounds3 get();
			}

			/// <summary>
			/// Gets or sets the contact offset.
			/// </summary>
			property float ContactOffset
			{
				float get();
				void set(float value);
			}

			/// <summary>
			/// Gets or sets the reset offset.
			/// </summary>
			property float RestOffset
			{
				float get();
				void set(float value);
			}

			/// <summary>
			/// Gets or sets the shape flags.
			/// </summary>
			property ShapeFlag Flags
			{
				ShapeFlag get();
				void set(ShapeFlag value);
			}

			/// <summary>
			/// Gets or sets the pose of the shape in actor space, i.e. relative to the actor they are owned by.
			/// </summary>
			property Matrix LocalPose
			{
				Matrix get();
				void set(Matrix value);
			}

			/// <summary>
			/// Gets or sets the user defined simulation filter data for this shape.
			/// </summary>
			property FilterData SimulationFilterData
			{
				FilterData get();
				void set(FilterData value);
			}

			/// <summary>
			/// Gets or sets the user defined query filter data for this shape.
			/// </summary>
			property FilterData QueryFilterData
			{
				FilterData get();
				void set(FilterData value);
			}

			/// <summary>Gets or sets an object, usually to create a 1:1 relationship with a user object.</summary>
			property Object^ UserData;

			/// <summary>Returns the associated geometry</summary>
			property Geometry^ Geom
			{
				Geometry^ get();
			}

			/// <summary>
			/// Sets the materials associated with this shape
			/// </summary>
			void SetMaterials(array<Material^>^ materials);

			/// <summary>
			/// Marks the object to reset interactions and re-run collision filters in the next simulation step
			/// </summary>
			void ResetFiltering();

		internal:
			property PxShape* UnmanagedPointer
			{
				PxShape* get();
			}

			property LinkedListNode<Shape^>^ ParentLink
			{
				void set(LinkedListNode<Shape^>^ link);
			}
	};
};