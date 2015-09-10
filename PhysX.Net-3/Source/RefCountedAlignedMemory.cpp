#include "StdAfx.h"
#include "RefCountedAlignedMemory.h"


namespace PhysX
{
	RefCountedAlignedMemory::RefCountedAlignedMemory(void* memBlock)
	{
		_count = 0;
		_block = memBlock;
	}


	RefCountedAlignedMemory::~RefCountedAlignedMemory()
	{
	}

	void RefCountedAlignedMemory::addRef()
	{
		++_count;
	}

	void RefCountedAlignedMemory::decRef()
	{
		if (_block != 0 && --_count <= 0) {
			// Free up the unmanaged memory
			_aligned_free(_block);
			_block = 0;

			//commit suicide
			delete this;
		}
	}
}