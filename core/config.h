#pragma once

#include <filesystem>
namespace fs = std::filesystem;
#include <variant>

#include "../base/base.h"
#include "../crypt/fnv1a.h"
#include "input.h"

template<class ty, class... ts>
concept any_of = (std::same_as<ty, ts> || ...);

namespace cfg {

    enum class item_type {
        i32,
        f32,
        boolean,
        clr3,
        clr4,
        bool_vec,
        keybind
    };

    template<class ty>
    concept configurable = any_of<ty, int, float, bool, clr3, clr4, std::vector<bool>, keybind>;

    struct item {
        std::variant<int, float, bool, clr3, clr4, std::vector<bool>, keybind> var{ };
        hash_t name{ };
        item_type type{ };

        constexpr item(configurable auto&& preset, hash_t name, item_type type) noexcept
            : var(preset), name(name), type(type) { }

        template<configurable ty>
        ty& get() noexcept
        {
            return std::get<ty>(var);
        }
    };

    inline std::vector<item> items{ };
    inline fs::path path{ };

    void initialize() noexcept;
    void read(std::wstring_view name) noexcept;
    void write(std::wstring_view name) noexcept;

    inline size_t add_item(item&& it) noexcept
    {
        items.push_back(it);
        return items.size() - 1;
    }

    template<class ty>
    ty& get(size_t idx) noexcept
    {
        return items[idx].get<ty>();
    }

}
