#pragma once

namespace cs {

template<class ty>
struct utl_vec {
    ty* data{ };
    int alloc_count{ };
    int grow_size{ };
    int size{ };
    ty* elements{ };

    constexpr ty& operator[](int i) noexcept
    {
        return data[i];
    }

    constexpr const ty& operator[](int i) const noexcept
    {
        return data[i];
    }
};

}
