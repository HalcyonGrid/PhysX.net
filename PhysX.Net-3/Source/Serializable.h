#pragma once

#include "SerializableEnum.h"

namespace PhysX
{
	ref class Collection;

	public ref class Serializable
	{
	private:
		PxSerializable* _serializable;

	internal:
		Serializable(PxSerializable* serializable);
	public:
		~Serializable();
	protected:
		!Serializable();

	public:
		property String^ ConcreteTypeName
		{
			String^ get();
		}

		property PhysX::ConcreteType ConcreteType
		{
			PhysX::ConcreteType get();
		}

		property int Order
		{
			int get();
		}

		void CollectForExport(Collection^ collection);
	};
};