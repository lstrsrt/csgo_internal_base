#include "interfaces.h"
#include "../crypt/xorstr.h"

static void collect_interfaces(dll& dll) noexcept;
template<bool replace_vmt = false, class ty>
static void get_cached_interface(interface_holder<ty*>& ptr, std::string_view version_string) noexcept;
template<bool replace_vmt = false, class ty>
static void find_interface(interface_holder<ty*>& ptr, dll& dll, std::string_view version_string) noexcept;

void interfaces::initialize() noexcept
{
    collect_interfaces(dlls::client);
    collect_interfaces(dlls::datacache);
    collect_interfaces(dlls::engine);
    collect_interfaces(dlls::file_system);
    collect_interfaces(dlls::input_system);
    collect_interfaces(dlls::localize);
    collect_interfaces(dlls::matchmaking);
    collect_interfaces(dlls::material_system);
    collect_interfaces(dlls::studio_render);
    collect_interfaces(dlls::vgui_mat_surface);
    collect_interfaces(dlls::vgui2);
    collect_interfaces(dlls::vphysics);
    collect_interfaces(dlls::vstdlib);

    get_cached_interface<true>(client, "VClient0");
    get_cached_interface(console, "GameConsole0");
    get_cached_interface(cvar, "VEngineCvar0");
    get_cached_interface(debug_overlay, "VDebugOverlay0");
    get_cached_interface(effects, "VEngineEffects0");
    get_cached_interface(effects_client, "IEffects0");
    get_cached_interface(engine, "VEngineClient0");
    get_cached_interface(entity_list, "VClientEntityList0");
    get_cached_interface(event_manager, "GAMEEVENTSMANAGER002");
    get_cached_interface(file_system, "VFileSystem0");
    get_cached_interface(game_types, "VENGINE_GAMETYPES_VERSION0");
    get_cached_interface(input_system, "InputSystemVersion0");
    get_cached_interface(leaf_system, "ClientLeafSystem0");
    get_cached_interface(localize, "Localize_0");
    get_cached_interface(match_framework, "MATCHFRAMEWORK_0");
    get_cached_interface(material_system, "VMaterialSystem0");
    get_cached_interface(mdl_cache, "MDLCache0");
    get_cached_interface(model_info, "VModelInfoClient0");
    get_cached_interface<true>(model_render, "VEngineModel0");
    get_cached_interface(movement, "GameMovement0");
    get_cached_interface(net_support, "INETSUPPORT_0");
    get_cached_interface(physics_props, "VPhysicsSurfaceProps0");
    get_cached_interface(prediction,"VClientPrediction0");
    get_cached_interface(render_view, "VEngineRenderView0");
    get_cached_interface(server, "ServerGameDLL0");
    get_cached_interface(sound, "IEngineSoundClient0");
    get_cached_interface(network_string_table, "VEngineClientStringTable0");
    get_cached_interface<true>(studio_render, "VStudioRender0");
    get_cached_interface(trace, "EngineTraceClient0");
    get_cached_interface(ui, "GameUI0");
    get_cached_interface<true>(vgui, "VEngineVGui0");

    // Not sure why but these only work manually...
    find_interface(panel, dlls::vgui2, "VGUI_Panel0");
    find_interface<true>(surface, dlls::vgui_mat_surface, "VGUI_Surface0");

    client_mode.initialize<true>(**memory::get_virtual(client, 10).offset(0x5).cast<se::client_mode***>());
    globals.initialize(**memory::get_virtual(client, 11).offset(0xa).cast<se::global_vars_base***>());
    input.initialize(*memory::get_virtual(client, 16).offset(0x1).cast<se::client_input**>());
    client_state.initialize(**memory::get_virtual(engine, 12).offset(0x10).cast<se::client_state***>());

    game_rules.initialize(**dlls::client.find(PATTERN("A1 ? ? ? ? 85 C0 0F 84 ? ? ? ? 80 B8 ? ? ? ? ? 74 7A")).offset(0x1).cast<se::game_rules***>());
    glow_manager.initialize(*dlls::client.find(PATTERN("0F 11 05 ? ? ? ? 83 C8 01")).offset(0x3).cast<se::glow_manager**>());
    move_helper.initialize(**dlls::client.find(PATTERN("8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01")).offset(0x2).cast<se::move_helper***>());
    player_resource.initialize(*dlls::client.find(PATTERN("8B 1D ? ? ? ? 89 5C 24 40")).offset(0x2).cast<se::player_resource**>());
    render_beams.initialize(*dlls::client.find(PATTERN("B9 ? ? ? ? FF 50 24 C2 04 00")).offset(0x1).cast<se::render_beams**>());
    weapon_system.initialize(*dlls::client.find(PATTERN("8B 35 ? ? ? ? FF 10 0F B7 C0")).offset(0x2).cast<se::weapon_system**>());
    view_render.initialize(**dlls::client.find(PATTERN("8B 0D ? ? ? ? D9 5D F0 8B 01")).offset(0x2).cast<se::view_render***>());
    dx9_device.initialize(**dlls::shader_api_dx9.find(PATTERN("A1 ? ? ? ? 50 8B 08 FF 51 0C")).offset(0x1).cast<IDirect3DDevice9***>());

    bsp_query.initialize(engine->get_bsp_tree_query());

    steam::user       = dlls::steam_api.get_export("SteamAPI_GetHSteamUser"_hash).cast<steam::h_user(*)()>()();
    steam::pipe       = dlls::steam_api.get_export("SteamAPI_GetHSteamPipe"_hash).cast<steam::h_pipe(*)()>()();
    steam_api_ctx     = engine->get_steam_api_context();
    steam_client      = steam_api_ctx->steam_client;
    game_coordinator  = static_cast<steam::game_coordinator*>(steam_client->get_generic_interface(steam::user, steam::pipe, "SteamGameCoordinator001"));
    steam_friends     = steam_api_ctx->steam_friends;
    matchmaking       = steam_api_ctx->steam_matchmaking;
    user_stats        = steam_api_ctx->steam_user_stats;

    mem_alloc         = *dlls::tier0.get_export("g_pMemAlloc"_hash).cast<se::mem_alloc**>();
    key_values_system = dlls::vstdlib.get_export("KeyValuesSystem"_hash).cast<se::key_values_system_fn>()();

    LOG_INFO("Interfaces initialized.");
}

namespace se {

struct interface_reg {
    std::add_pointer_t<void*()> create_fn{ };
    const char* name{ };
    interface_reg* next{ };
};

}

static auto get_interface_regs(dll& dll) noexcept
{
    if (!dll.create_interface) {
        dll.create_interface = dll.get_export("CreateInterface"_hash);
        VERIFY(dll.create_interface);
    }

    // Follow jmp instruction inside function to get to CreateInterfaceInternal(), where the global interface list is moved into ESI.
    const auto reg_list = **dll.create_interface.absolute<se::interface_reg***>(0x5, 0x6);
    VERIFY(reg_list);
    return reg_list;
}

static void collect_interfaces(dll& dll) noexcept
{
    for (auto cur = get_interface_regs(dll); cur; cur = cur->next) {
        LOG_INFO("{}: found interface {}", dll.name, cur->name);
        interfaces::list.push_back(std::make_pair(cur->name, cur->create_fn()));
    }
}

// Version strings may be partial.

template<bool replace_vmt, class ty>
static void find_interface(interface_holder<ty*>& ptr, dll& dll, std::string_view version_string) noexcept
{
    for (auto cur = get_interface_regs(dll); cur; cur = cur->next) {
        if (std::string(cur->name).starts_with(version_string)) {
            LOG_SUCCESS("Found interface {}.", cur->name);
            ptr.template initialize<replace_vmt>(static_cast<ty*>(cur->create_fn()));
            return;
        }
    }
    VERIFY(false);
}

template<bool replace_vmt, class ty>
static void get_cached_interface(interface_holder<ty*>& ptr, std::string_view version_string) noexcept
{
    for (const auto& [name, iface] : interfaces::list) {
        if (name.starts_with(version_string.data())) {
            ptr.template initialize<replace_vmt>(static_cast<ty*>(iface));
            return;
        }
    }
}
