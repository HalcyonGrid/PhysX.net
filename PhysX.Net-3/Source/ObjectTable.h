#pragma once 

#include "IDisposable.h"
#include "ObjectTableOwnershipType.h"
#include "ObjectTableEventArgs.h"

namespace PhysX
{
	ref class ObjectTableEventArgs;

	public ref class OwnershipInfo sealed
	{
	public:
		PhysX::IDisposable^ Owner;
		HashSet<PhysX::IDisposable^>^ Owns;
		OwnershipInfo^ Parent;

		OwnershipInfo(PhysX::IDisposable^ owner, OwnershipInfo^ parent)
		{
			this->Owner = owner;
			this->Parent = parent;
			this->Owns = gcnew HashSet<IDisposable^>();
		}

		void ChildRemoved(PhysX::IDisposable^ child)
		{
			this->Owns->Remove(child);
		}
	};
	
	// TODO: Make ObjectTable an instance class instead of containing all static data and methods, but then wrap in singleton pattern.
	/// <summary>
	/// Manages lookups and disposals for unmanaged-managed pairs.
	/// This class is not thread safe.
	/// </summary>
	public ref class ObjectTable sealed
	{
		public:
			static event EventHandler<ObjectTableEventArgs^>^ ObjectAdded;
			static event EventHandler<ObjectTableEventArgs^>^ ObjectRemoved;
			
		private:
			// A collection of native objects to their managed version
			static Dictionary<intptr_t, Object^>^ _objectTable;
			static Dictionary<Object^, intptr_t>^ _objectTableRev;

			// A collection of managed objects to their managed owner
			static Dictionary<PhysX::IDisposable^, OwnershipInfo^>^ _ownership;
			// A collection of ownership-type pairs to a collection of objects
			// This dictionary is used to lookup objects which are owned by X and of type Y. (e.g. property Physics.Cloths > Key: Owner: physics, Type: Cloth yields a collection of Cloth).
			static Dictionary<ObjectTableOwnershipType, List<Object^>^>^ _ownerTypeLookup;
			
			static bool _performingDisposal;
			
		private:
			ObjectTable();
			static ObjectTable()
			{
				_objectTable = gcnew Dictionary<intptr_t, Object^>();
				_objectTableRev = gcnew Dictionary<Object^, intptr_t>();
				_ownership = gcnew Dictionary<PhysX::IDisposable^, OwnershipInfo^>();
				_ownerTypeLookup = gcnew Dictionary<ObjectTableOwnershipType, List<Object^>^>();
				
				_performingDisposal = false;
			}
			
		public:
			generic<typename T> where T : PhysX::IDisposable
			static void Add(intptr_t pointer, T object, PhysX::IDisposable^ owner);
			static void EnsureUnmanagedObjectIsOnlyWrappedOnce(intptr_t unmanaged, Type^ managedType);
			
			static void AddObjectOwner(PhysX::IDisposable^ object, PhysX::IDisposable^ owner);
			generic<typename T> where T : PhysX::IDisposable
			static void AddOwnerTypeLookup(Object^ owner, T object);

			static bool Remove(intptr_t pointer);
			static bool Remove(Object^ object);
				
			static void Clear();
			
			static Object^ GetObject(intptr_t pointer);
			generic<typename T> static T GetObject(intptr_t pointer);
			
			template<typename T> static T TryGetObject(intptr_t pointer)
			{
				Object^ retObj;
				if (_objectTable->TryGetValue(pointer, retObj))
				{
					return dynamic_cast<T>(retObj);
				}

				return T();
			}

			static intptr_t GetObject(Object^ object);

			generic<typename T>
			static array<T>^ GetObjects(intptr_t* pointers, int count);
			
			generic<typename T> where T : ref class
			static array<T>^ GetObjectsOfType();

			generic<typename T> where T : ref class
			static IEnumerable<T>^ GetObjectsOfOwnerAndType(Object^ owner);

			static bool Contains(intptr_t pointer);
			static bool Contains(Object^ object);
			
		private:
			static void disposableObject_OnDisposing(Object^ sender, EventArgs^ e);

			static void DisposeOfObjectAndDependents(PhysX::IDisposable^ disposable);

			static List<PhysX::IDisposable^>^ GetDependents(PhysX::IDisposable^ disposable);
			static void GetDependents(PhysX::IDisposable^ disposable, List<PhysX::IDisposable^>^ disposables);
			
		public:
			property int Count
			{
				static int get();
			}

			property Dictionary<intptr_t, Object^>^ Objects
			{
				static Dictionary<intptr_t, Object^>^ get();
			}
			property Dictionary<PhysX::IDisposable^, OwnershipInfo^>^ Ownership
			{
				static Dictionary<PhysX::IDisposable^, OwnershipInfo^>^ get();
			}
			property Dictionary<ObjectTableOwnershipType, List<Object^>^>^ OwnerTypeLookup
			{
				static Dictionary<ObjectTableOwnershipType, List<Object^>^>^ get();
			}
	};
};