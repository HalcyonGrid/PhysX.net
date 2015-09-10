#include "StdAfx.h"

#include "Serializable.h"
#include "Collection.h"

Serializable::Serializable(PxSerializable* serializable)
{
	_serializable = serializable;
}
Serializable::~Serializable()
{
	this->!Serializable();
}
Serializable::!Serializable()
{
	_serializable = NULL;
}

String^ Serializable::ConcreteTypeName::get()
{
	return Util::ToManagedString(_serializable->getConcreteTypeName());
}

PhysX::ConcreteType Serializable::ConcreteType::get()
{
	return ToManagedEnum(PhysX::ConcreteType, _serializable->getConcreteType());
}

int Serializable::Order::get()
{
	return _serializable->getOrder();
}

void Serializable::CollectForExport(Collection^ collection)
{
	_serializable->collectForExport(*collection->UnmanagedPointer);
}