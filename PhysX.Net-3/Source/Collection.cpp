#include "StdAfx.h"
#include "Collection.h"
#include "Physics.h"
#include "Scene.h"
#include "Serializable.h"

#include "TriangleMesh.h"
#include "ConvexMesh.h"
#include "RefCountedAlignedMemory.h"

#include <PxExtensionsAPI.h>
#include <PxSerialFramework.h> 
#include <PxTriangleMesh.h>
#include <PxConvexMesh.h>

Collection::Collection(PxCollection* collection, PhysX::Physics^ owner)
{
	if (collection == NULL)
		throw gcnew ArgumentNullException("collection");
	ThrowIfNullOrDisposed(owner, "owner");

	_collection = collection;
	_physics = owner;

	_deserializedObjects = gcnew List<Object^>();

	//ObjectTable::Add((intptr_t)collection, this, owner);
}
Collection::~Collection()
{
	this->!Collection();
}
Collection::!Collection()
{
	OnDisposing(this, nullptr);

	if (Disposed)
		return;

	this->Physics->UnmanagedPointer->releaseCollection(*_collection);
	_collection = NULL;

	_physics = nullptr;

	OnDisposed(this, nullptr);
}

bool Collection::Disposed::get()
{
	return _collection == NULL;
}

void Collection::Serialize(System::IO::Stream^ stream)
{
	ThrowIfNull(stream, "stream");
	if (!stream->CanWrite)
		throw gcnew ArgumentException("Cannot write to stream", "stream");

	PxDefaultMemoryOutputStream s;

	_collection->serialize(s);

	PxU32 size = s.getSize();

	if (size <= 0)
		return;

	PxU8* data = s.getData();

	array<Byte>^ buffer = gcnew array<Byte>(size);
	pin_ptr<Byte> b = &buffer[0];
	memcpy_s(b, size, (void*)data, size);

	stream->Write(buffer, 0, size);
}
bool Collection::Deserialize(System::IO::Stream^ stream)
{
	ThrowIfNull(stream, "stream");
	if (!stream->CanRead)
		throw gcnew ArgumentException("Cannot read from stream", "stream");

	int n = (int)stream->Length;

	if (n <= 0)
		return false;

	// Read the entire contents of the stream to a managed byte array
	array<Byte>^ data = gcnew array<Byte>(n);
	stream->Read(data, 0, n);
	// Pin the first element
	pin_ptr<Byte> d = &data[0];

	// Allocate a 128 byte aligned block of memory and copy in the stream data
	void* j = _aligned_malloc(n, 128);
	memcpy_s(j, n, d, n);
	
	// Deserialize
	bool result = _collection->deserialize(j, NULL, NULL);

	FillDeserializedList(j);

	return result;
}

int Collection::GetNumberOfObjects()
{
	return _collection->getNbObjects();
}

//

void Collection::CollectPhysicsForExport(PhysX::Physics^ physics)
{
	ThrowIfNullOrDisposed(physics, "physics");

	PxCollectForExportSDK(*physics->UnmanagedPointer, *_collection);
}
void Collection::CollectSceneForExport(Scene^ scene)
{
	ThrowIfNullOrDisposed(scene, "scene");

	PxCollectForExportScene(*scene->UnmanagedPointer, *_collection);
}

//

PhysX::Physics^ Collection::Physics::get()
{
	return _physics;
}

PxCollection* Collection::UnmanagedPointer::get()
{
	return _collection;
}

IList<Object^>^ Collection::DeserializedObjects::get()
{
	return _deserializedObjects;
}

void Collection::FillDeserializedList(void* memoryBlock)
{
	PxU32 numObjects = _collection->getNbObjects();
	if (numObjects == 0) return;

	RefCountedAlignedMemory* block = new RefCountedAlignedMemory(memoryBlock);

	for (PxU32 i = 0; i < numObjects; i++)
	{
		PxSerializable* ser = _collection->getObject(i);

		// we only support triangle mesh and convex mesh
		switch (ser->getConcreteType())
		{
		case PxConcreteType::eTRIANGLE_MESH: 
			{
				PxTriangleMesh* trimesh = ser->is<PxTriangleMesh>();
				_deserializedObjects->Add(gcnew PhysX::TriangleMesh(trimesh, _physics, block));
			}
			break;

		case PxConcreteType::eCONVEX_MESH:
			{
				PxConvexMesh* convexMesh = ser->is<PxConvexMesh>();
				_deserializedObjects->Add(gcnew PhysX::ConvexMesh(convexMesh, _physics, block));
			}
			break;
		}
	}
}