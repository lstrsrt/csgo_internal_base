#include "key_values.h"
#include "../../memory/memory.h"
#include "../../memory/interface.h"

namespace cs {

	void* key_values::operator new[](size_t alloc_size)
	{
		return memory::call_virtual<void*, 2>(interfaces::key_values_system, alloc_size);
	}

	void key_values::operator delete[](void* mem)
	{
		return memory::call_virtual<void, 3>(interfaces::key_values_system, mem);
	}

}
