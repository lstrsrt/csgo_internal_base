#include "../hooks.h"
#include "../features/cache.h"
#include "../features/features.h"

void __fastcall hooks::level_init_post_entity::fn(se::client_dll* ecx, int)
{
    cache::initialize();
    local.update();

    return original(ecx);
}

void __fastcall hooks::level_shutdown::fn(se::client_dll* ecx, int)
{
    cache::clear();
    local.reset();

    return original(ecx);
}

static void __stdcall create_move(int sequence_nr, float input_sample_time, bool is_active, bool& send_packet)
{
    hooks::create_move_proxy::original(interfaces::client, sequence_nr, input_sample_time, is_active);

    if (!interfaces::game_rules)
        interfaces::game_rules = **dlls::client.find(PATTERN("A1 ? ? ? ? 85 C0 0F 84 ? ? ? ? 80 B8 ? ? ? ? ? 74 7A"))
        .offset(0x1).cast<se::game_rules***>();

    if (!is_active || !send_packet)
        return;

    if (!local.update())
        return;

    auto cmd = &interfaces::input->cmds[sequence_nr % cs::multiplayer_backup];
    auto verified_cmd = &interfaces::input->verified_cmds[sequence_nr % cs::multiplayer_backup];
    if (!cmd || !verified_cmd)
        return;

    local.cur_cmd = cmd;

    if (cfg::get<bool>(vars::infinite_crouch))
        cmd->buttons.set(cs::cmd_button::bullrush);

    // aimbot::run(cmd);

    local.view = cmd->view_angles;

    verified_cmd->cmd = *cmd;
    verified_cmd->crc = cmd->get_checksum();
}

__declspec(naked) void __fastcall hooks::create_move_proxy::fn(se::client_dll* ecx, int, int sequence_nr,
    float input_sample_time, bool is_active)
{
    __asm {
        push ebp
        mov  ebp, esp

        push  ebx // send_packet
        push  esp
        push  dword ptr[ebp + 16]
        push  dword ptr[ebp + 12]
        push  dword ptr[ebp + 8]
        call  create_move
        pop   ebx

        pop   ebp
        ret   12
    }
}

void __fastcall hooks::frame_stage_notify::fn(se::client_dll* ecx, int, cs::frame_stage frame_stage)
{
    if (!local.in_game)
        return original(ecx, frame_stage);

    static auto override_postprocessing_disable = *dlls::client.find(PATTERN("83 EC 4C 80 3D")).offset(0x5).cast<bool**>();

    switch (frame_stage) {
    case cs::frame_stage::render_start:
        *override_postprocessing_disable = cfg::get<bool>(vars::disable_postprocessing);
        break;
    case cs::frame_stage::net_update_post_data_update_end:
        local.update();
        break;
    default:
        break;
    }

    return original(ecx, frame_stage);
}
