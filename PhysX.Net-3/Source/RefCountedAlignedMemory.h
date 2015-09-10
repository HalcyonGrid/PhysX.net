#pragma once

namespace PhysX
{
	class RefCountedAlignedMemory
	{
	private:
		void* _block;
		int _count;

	public:
		RefCountedAlignedMemory(void* memBlock);
		~RefCountedAlignedMemory();

		void addRef();
		void decRef();
	};
}

