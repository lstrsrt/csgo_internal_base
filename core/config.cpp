#include "config.h"
#include "../base/debug.h"

void cfg::initialize() noexcept
{
    if (!fs::exists("csgo_internal_base"))
        fs::create_directory("csgo_internal_base");

    path = fs::current_path() / "csgo_internal_base";

    if (!fs::exists(path / "default.cfg"))
        write(L"default.cfg");

    read(L"default.cfg");
}

void cfg::read(std::wstring_view name) noexcept
{
    const auto index_from_hash = [](const hash_t name) {
        for (size_t i{ }; i < items.size(); i++) {
            if (items[i].name == name)
                return i;
        }
        VERIFY(false);
        return std::numeric_limits<hash_t>::max();
    };

    const auto cfg_path = path / name.data();
    std::ifstream cfg{ cfg_path };
    if (!cfg)
        LOG_ERROR(L"Could not open config {} for reading!", cfg_path.c_str());
    else {
        std::string line{ };
        // Separate into lines
        while (getline(cfg, line)) {
            // Separate into tokens
            std::istringstream stream{ line };
            hash_t hash{ };
            stream >> hash;
            auto& cur_item = items[index_from_hash(hash)];

            std::string token{ };
            stream >> token; // Type
            switch (fnv1a::hash(token)) {
            case "i32"_hash:
                stream >> cur_item.get<int32_t>();
                break;
            case "f32"_hash:
                stream >> cur_item.get<float_t>();
                break;
            case "bool"_hash:
                stream >> cur_item.get<bool>();
                break;
            case "c3"_hash:
                stream >> cur_item.get<clr3>().r >> cur_item.get<clr3>().g >> cur_item.get<clr3>().b;
                break;
            case "c4"_hash: {
                auto& clr = cur_item.get<clr4>();
                /* This abomination forces the stream to read the whole number instead of one char. */
                int tmp{ };
                stream >> tmp; clr.r = static_cast<uint8_t>(tmp);
                stream >> tmp; clr.g = static_cast<uint8_t>(tmp);
                stream >> tmp; clr.b = static_cast<uint8_t>(tmp);
                stream >> tmp; clr.a = static_cast<uint8_t>(tmp);
                break;
            }
            case "vec"_hash: {
                stream.ignore(); // Skip space
                auto& vec = cur_item.get<std::vector<bool>>();
                for (size_t i{ }; i < vec.size(); i++)
                    vec[i] = (stream.get() == '1'); // Anything other than 1 is false
                break;
            }
            case "key"_hash: {
                auto& key = cur_item.get<keybind>();
                stream >> key.code;
                int type{ };
                stream >> type;
                key.type = static_cast<input::key_type>(type);
                break;
            }
            default:
                LOG_ERROR("Unknown config item type!");
            }
        }
        LOG_INFO(L"Read from {}.", name);
    }
}

void cfg::write(std::wstring_view name) noexcept
{
    const auto cfg_path = path / name.data();
    std::ofstream cfg{ cfg_path, std::ios::out | std::ios::trunc };
    if (!cfg)
        LOG_ERROR(L"Could not open config {} for writing!", cfg_path.c_str());
    else {
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
                cfg << "c3 " << static_cast<int>(clr.r) << ' ' << static_cast<int>(clr.g) << ' '
                    << static_cast<int>(clr.b) << '\n';
                break;
            }
            case item_type::clr4: {
                const auto& clr = var.get<clr4>();
                cfg << "c4 " << static_cast<int>(clr.r) << ' ' << static_cast<int>(clr.g) << ' '
                    << static_cast<int>(clr.b) << ' ' << static_cast<int>(clr.a) << '\n';
                break;
            }
            case item_type::bool_vec:
                cfg << "vec ";
                for (const auto& b : var.get<std::vector<bool>>())
                    cfg << static_cast<int>(b);
                cfg << '\n';
                break;
            case item_type::keybind: {
                const auto& key = var.get<keybind>();
                cfg << "key " << key.code << ' ' << std::to_underlying(key.type) << '\n';
                break;
            }
            }
        }
        LOG_INFO(L"Wrote to {}.", name);
    }
}
