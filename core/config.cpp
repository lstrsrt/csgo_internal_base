#include "config.h"

void config::initialize() noexcept
{
    if (!std::filesystem::exists("csgo_internal_base"))
        std::filesystem::create_directory("csgo_internal_base");

    path = std::filesystem::current_path() / "csgo_internal_base";

    if (!std::filesystem::exists(path / "default.cfg"))
        write(L"default.cfg");

    read(L"default.cfg");
}

void config::read(std::wstring_view name) noexcept
{
    const auto index_from_hash = [](const hash_t name) {
        for (size_t i = 0; i < items.size(); i++) {
            if (items[i].name == name)
                return i;
        }
        LOG_ERROR("Could not get a config item index for hash {}!", name);
        return std::numeric_limits<hash_t>::max();
    };

    std::fstream cfg{ path / name.data(), std::fstream::in };
    if (cfg.good()) {
        std::string line{ };
        // Separate into lines
        while (getline(cfg, line)) {
            // Separate into tokens
            std::istringstream stream{ line };
            hash_t hash{ };
            stream >> hash;
            item& cur_item = items[index_from_hash(hash)];

            std::string token{ };
            stream >> token; // Type
            switch (type_table.at(token)) {
            case item_type::i32:
                stream >> cur_item.get<int32_t>();
                break;
            case item_type::f32:
                stream >> cur_item.get<float_t>();
                break;
            case item_type::boolean:
                stream >> cur_item.get<bool>();
                break;
            case item_type::clr3:
                stream >> cur_item.get<clr3>().r >> cur_item.get<clr3>().g >> cur_item.get<clr3>().b;
                break;
            case item_type::clr4:
                stream >> cur_item.get<clr4>().r >> cur_item.get<clr4>().g >> cur_item.get<clr4>().b >> cur_item.get<clr4>().a;
                break;
            case item_type::bool_vec:
                while (getline(stream, token, ' ')) {
                    // TODO
                }
                break;
            case item_type::keybind:
                stream >> cur_item.get<keybind>().code;
                auto type = static_cast<int>(cur_item.get<keybind>().type);
                stream >> type;
                break;
            }
        }
    }
    LOG_INFO(L"Read from {}.", name);
}

void config::write(std::wstring_view name) noexcept
{
    std::fstream cfg{ path / name.data(), std::fstream::out | std::fstream::trunc };
    if (cfg.good()) {
        for (auto& var : items) {
            cfg << var.name << ' ';
            switch (var.type) {
            case item_type::i32:
                cfg << "i32 " << var.get<int32_t>() << '\n';
                break;
            case item_type::f32:
                cfg << "f32 " << var.get<float_t>() << '\n';
                break;
            case item_type::boolean:
                cfg << "bool " << var.get<bool>() << '\n';
                break;
            case item_type::clr3: {
                const auto& clr = var.get<clr3>();
                cfg << "c3 " << (int)clr.r << ' ' << (int)clr.g << ' ' << (int)clr.b << '\n';
                break;
            }
            case item_type::clr4: {
                const auto& clr = var.get<clr4>();
                cfg << "c4 " << (int)clr.r << ' ' << (int)clr.g << ' ' << (int)clr.b << ' ' << (int)clr.a << '\n';
                break;
            }
            case item_type::bool_vec:
                cfg << "vec ";
                for (const auto& a : var.get<std::vector<bool>>())
                    cfg << a << ' ';
                cfg << '\n';
                break;
            case item_type::keybind: {
                const auto& key = var.get<keybind>();
                cfg << "key " << key.code << ' ' << to_underlying(key.type);
                break;
            }
            }
        }
    }
    LOG_INFO(L"Wrote to {}.", name);
}
