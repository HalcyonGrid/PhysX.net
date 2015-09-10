#include "StdAfx.h"
#include "ObjectTable.h"
#include "IDisposable.h"
#include "PhysXException.h"

using namespace System::Threading;
using namespace System::Linq;

ObjectTable::ObjectTable()
{

}

// Add
generic<typename T>
void ObjectTable::Add(intptr_t pointer, T object, PhysX::IDisposable^ owner)
{
	if (pointer == NULL)
		throw gcnew PhysXException("Invalid pointer added to Object Table", "object");
	if (object == nullptr)
		throw gcnew ArgumentNullException("Invalid pointer added to Object Table", "owner");
	
	// Make sure we have not constructed a new managed object around an unmanaged pointer more than once
	// This leads to a case where one of the objects will be disposed, and the other(s) unaware of this and thus
	// holding on to broken pointers.
	// Unmanaged objects should only be wrapped once, then retrieved from the ObjectTable each time after.
	EnsureUnmanagedObjectIsOnlyWrappedOnce(pointer, object->GetType());

	AddObjectOwner(object, owner);
	AddOwnerTypeLookup<T>(owner, object);
		
	try
	{
		_objectTable->Add(pointer, object);
		_objectTableRev->Add(object, pointer);
	}
	catch (Exception^)
	{
		throw;
	}

	ObjectAdded(nullptr, gcnew ObjectTableEventArgs(pointer, object));
}

void ObjectTable::AddObjectOwner(PhysX::IDisposable^ object, PhysX::IDisposable^ owner)
{
	if (object == nullptr)
		throw gcnew ArgumentNullException("object");

	object->OnDisposing += gcnew EventHandler(&ObjectTable::disposableObject_OnDisposing);

	OwnershipInfo^ ownerInfo = nullptr;
	if (owner != nullptr)
	{
		if (_ownership->TryGetValue(owner, ownerInfo))
		{
			ownerInfo->Owns->Add(object);
		}
	}

	_ownership->Add(object, gcnew OwnershipInfo(owner, ownerInfo));
}
generic<typename T>

void ObjectTable::AddOwnerTypeLookup(Object^ owner, T object)
{
	if (object == nullptr)
		throw gcnew ArgumentNullException("object");

	Type^ type = object->GetType();

	auto key = ObjectTableOwnershipType(owner, type);

	if (!_ownerTypeLookup->ContainsKey(key))
	{
		_ownerTypeLookup->Add(key, gcnew List<Object^>());
	}

	_ownerTypeLookup[key]->Add(object);
}

void ObjectTable::EnsureUnmanagedObjectIsOnlyWrappedOnce(intptr_t unmanaged, Type^ managedType)
{
	if (!_objectTable->ContainsKey(unmanaged))
		return;

	auto obj = _objectTable[unmanaged];

	if (obj->GetType() == managedType)
		throw gcnew InvalidOperationException(String::Format("There is already a managed instance of type '{0}' wrapping this unmanaged object. Instead retrieve the managed object from the ObjectTable using the unmanaged pointer as the lookup key.", managedType->FullName));
}

// Remove
bool ObjectTable::Remove(intptr_t pointer)
{
	Object^ object = _objectTable[pointer];
		
	// Unbind the OnDisposing event
	if (IsInstanceOf<PhysX::IDisposable^>(object))
	{
		PhysX::IDisposable^ disposableObject = dynamic_cast<PhysX::IDisposable^>(object);
			
		disposableObject->OnDisposing -= gcnew EventHandler(&ObjectTable::disposableObject_OnDisposing);
	}
		
	// Remove from the pointer-object dictionary
	bool result = _objectTable->Remove(pointer);
	_objectTableRev->Remove(object);
		
	// Remove the from owner-type dictionary
	if (IsInstanceOf<PhysX::IDisposable^>(object))
	{
		IDisposable^ disposableObject = dynamic_cast<PhysX::IDisposable^>(object);

		IDisposable^ owner = _ownership[disposableObject]->Owner;

		auto ownerTypeKey = ObjectTableOwnershipType(owner, object->GetType());

		if (_ownerTypeLookup->ContainsKey(ownerTypeKey))
		{
			_ownerTypeLookup[ownerTypeKey]->Remove(object);

			if (_ownerTypeLookup[ownerTypeKey]->Count == 0)
				_ownerTypeLookup->Remove(ownerTypeKey);
		}
	}

	// Remove from the ownership dictionary
	if (IsInstanceOf<PhysX::IDisposable^>(object))
	{
		PhysX::IDisposable^ disposableObject = dynamic_cast<PhysX::IDisposable^>(object);

		OwnershipInfo^ ownInfo;
		if (_ownership->TryGetValue(disposableObject, ownInfo))
		{
			if (ownInfo->Parent != nullptr)
			{
				ownInfo->Parent->ChildRemoved(disposableObject);
			}

			_ownership->Remove(disposableObject);
		}
	}

	// Raise event
	ObjectRemoved(nullptr, gcnew ObjectTableEventArgs(pointer, object));
		
	return result;
}
bool ObjectTable::Remove(Object^ object)
{
	intptr_t objPtr;
	if (_objectTableRev->TryGetValue(object, objPtr))
	{
		return Remove(objPtr);
	}
	else
	{
		return false;
	}
}

void ObjectTable::Clear()
{
	_objectTable->Clear();
	_objectTableRev->Clear();
	_ownership->Clear();
	_ownerTypeLookup->Clear();
}

//

int ObjectTable::Count::get()
{
	return _objectTable->Count;
}

Dictionary<intptr_t, Object^>^ ObjectTable::Objects::get()
{
	return _objectTable;
}
Dictionary<PhysX::IDisposable^, OwnershipInfo^>^ ObjectTable::Ownership::get()
{
	return _ownership;
}
Dictionary<ObjectTableOwnershipType, List<Object^>^>^ ObjectTable::OwnerTypeLookup::get()
{
	return _ownerTypeLookup;
}