#pragma once

#include <array>

namespace detour {

    class hook {
    public:
        explicit hook(void* target, void* detour, void** original) noexcept
            : target(target), hook_fn(detour), original(original) { }

        bool set(bool enable = true) noexcept;
        void enable() noexcept;
        void disable() noexcept;
        void remove() noexcept;

    private:
        void* target{ };
        void* hook_fn{ };
        void** original{ };
        void* trampoline{ };
        std::array<uint8_t, 5> real_bytes{ };
    };

}
