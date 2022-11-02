#pragma once

#include <any>
#include <filesystem>
#include <unordered_map>

#include "../base/base.h"
#include "../crypt/fnv1a.h"
#include "input.h"

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

    struct item {
        std::any var{ };
        hash_t name{ };
        item_type type{ };

        template<typename ty>
        ty& get() noexcept
        {
            return *static_cast<ty*>(std::any_cast<ty>(&var));
        }
    };

    inline std::vector<item> items{ }; // Currently used variables
    inline std::vector<std::string> list{ }; // Config names
    inline std::filesystem::path path{ };
    const inline std::unordered_map<std::string, item_type> type_table{
        { "i32", item_type::i32 },
        { "f32", item_type::f32 },
        { "bool", item_type::boolean },
        { "c3", item_type::clr3 },
        { "c4", item_type::clr4 },
        { "vec", item_type::bool_vec },
        { "key", item_type::keybind }
    };

    void initialize() noexcept;
    void read(std::wstring_view name) noexcept;
    void write(std::wstring_view name) noexcept;

    inline size_t add_item(const item&& it) noexcept
    {
        items.push_back(it);
        return items.size() - 1;
    }

    template<class ty>
    ty& get(const size_t index) noexcept
    {
        return items[index].get<ty>();
    }

}
