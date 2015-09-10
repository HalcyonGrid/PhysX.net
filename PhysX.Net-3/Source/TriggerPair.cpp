#include "StdAfx.h"
#include "TriggerPair.h"
#include "Shape.h"
#include "Actor.h"

PxTriggerPair TriggerPair::ToUnmanaged(TriggerPair^ pair)
{
	ThrowIfNull(pair, "pair");

	PxTriggerPair tp;
		tp.otherShape = GetPointerOrNull(pair->OtherShape);
		tp.status = ToUnmanagedEnum(PxPairFlag, pair->Status);
		tp.triggerShape = GetPointerOrNull(pair->TriggerShape);

	return tp;
}
TriggerPair^ TriggerPair::ToManaged(PxTriggerPair pair)
{
	auto tp = gcnew TriggerPair();
		tp->OtherShape = ObjectTable::TryGetObject<Shape^>((intptr_t)pair.otherShape);

		//if a touch_lost is reported the otherShape may be deleted, so dont try to dereference
		if ((pair.status & PxPairFlag::eNOTIFY_TOUCH_LOST) == false)
		{
			tp->OtherActor = ObjectTable::TryGetObject<Actor^>((intptr_t)(&pair.otherShape->getActor()));
		}

		tp->Status = ToManagedEnum(PairFlag, pair.status);
		tp->TriggerShape = ObjectTable::TryGetObject<Shape^>((intptr_t)pair.triggerShape);

	return tp;
}