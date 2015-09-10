#include "StdAfx.h"
#include "RigidActor.h"
#include "Geometry.h"
#include "Material.h"
#include "Shape.h"
#include "ShapeCreationException.h"
#include "Physics.h"
#include "RigidStatic.h"
#include <PxRigidActor.h>
#include <PxShape.h>
#include <PxBoxGeometry.h>
#include <PxSimpleFactory.h>

RigidActor::RigidActor(PxRigidActor* rigidActor, PhysX::Physics^ owner)
	: Actor(rigidActor, owner)
{
	_shapes = gcnew LinkedList<Shape^>();
}
RigidActor::~RigidActor()
{
	
}
RigidActor::!RigidActor()
{
	
}

void RigidActor::TrackShape(Shape^ shape)
{
	ThrowIfNull(shape, "shape");

	_shapes->AddLast(shape);
}

Shape^ RigidActor::CreateShape(Geometry^ geometry, Material^ material, [Optional] Nullable<Matrix> localPose)
{
	ThrowIfNull(geometry, "geometry");
	ThrowIfNullOrDisposed(material, "material");

	Matrix pose = localPose.GetValueOrDefault(Matrix::Identity);

	PxGeometry* geom = geometry->ToUnmanaged();

	PxShape* s = this->UnmanagedPointer->createShape(*geom, *material->UnmanagedPointer, MathUtil::MatrixToPxTransform(pose));

	if (s == NULL)
		throw gcnew ShapeCreationException("Failed to create shape");

	// PxGeometry instances are created when we call ToUnmanaged()
	SAFE_DELETE(geom);

	Shape^ shape = gcnew Shape(s, this, geometry);

	LinkedListNode<Shape^>^ link = _shapes->AddLast(shape);
	shape->ParentLink = link;

	return shape;
}

void RigidActor::Scale(float scale)
{
	Scale(scale, true);
}
void RigidActor::Scale(float scale, bool scaleMassProperties)
{
	PxScaleRigidActor(*this->UnmanagedPointer, scale, scaleMassProperties);
}

RigidStatic^ RigidActor::CloneStatic(Matrix transform)
{
	PxRigidStatic* rigidStatic = PxCloneStatic(
		*this->Physics->UnmanagedPointer, 
		MathUtil::MatrixToPxTransform(transform),
		*this->UnmanagedPointer);

	return gcnew RigidStatic(rigidStatic, this->Physics);
}

Matrix RigidActor::GlobalPose::get()
{
	return MathUtil::PxTransformToMatrix(&this->UnmanagedPointer->getGlobalPose());
}
void RigidActor::GlobalPose::set(Matrix value)
{
	return this->UnmanagedPointer->setGlobalPose(MathUtil::MatrixToPxTransform(value));
}

IEnumerable<Shape^>^ RigidActor::Shapes::get()
{
	return _shapes;
}

PxRigidActor* RigidActor::UnmanagedPointer::get()
{
	return (PxRigidActor*)Actor::UnmanagedPointer;
}

void RigidActor::ChildShapeRemoved(LinkedListNode<Shape^>^ shape)
{
	_shapes->Remove(shape);
}