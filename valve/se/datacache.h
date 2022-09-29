#pragma once

namespace cs { using mdl_handle = unsigned short; }

namespace se {

struct mdl_cache {
    VIRTUAL_FUNCTION(find_model, cs::mdl_handle, 11, (const char* relative_path), (this, relative_path))
    VIRTUAL_FUNCTION(begin_lock, void, 33, (), (this))
    VIRTUAL_FUNCTION(end_lock, void, 34, (), (this))
};

class mdl_cache_critical_section {
public:
    explicit mdl_cache_critical_section(mdl_cache* ptr) noexcept
        : cache(ptr)
    {
        cache->begin_lock();
    }

    ~mdl_cache_critical_section()
    {
        cache->end_lock();
    }

private:
    mdl_cache* cache{ };
};

}
