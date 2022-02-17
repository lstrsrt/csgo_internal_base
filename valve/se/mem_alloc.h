#pragma once

namespace se {

struct mem_alloc {
    template<class ty>
    VIRTUAL_FUNCTION(alloc, ty, 1, (size_t size), (this, size))
    template<class ty>
    VIRTUAL_FUNCTION(realloc, ty, 3, (ty mem, size_t size), (this, mem, size))
    template<class ty>
    VIRTUAL_FUNCTION(free, void, 5, (ty mem), (this, mem))
};

}
