#include "features.h"

static void update() noexcept
{
    if (interfaces::client_state->delta_tick > 0)
    interfaces::prediction->update(interfaces::client_state->delta_tick, true, interfaces::client_state->last_cmd_acknowledged,
        interfaces::client_state->last_outgoing_cmd + interfaces::client_state->choked_cmds);
}

static void update_button_state(bitfield<cs::cmd_button>& cmd_buttons) noexcept
{
    auto buttons = local_player->get_buttons();
    local_player->get_button_last() = *buttons;

    *buttons = cmd_buttons;
    const auto buttons_changed = local_player->get_button_last().to_underlying() ^ buttons->to_underlying();
    
    local_player->get_button_pressed() = buttons_changed & buttons->to_underlying();
    local_player->get_button_released() = buttons_changed & (~buttons->to_underlying());
}

void features::prediction::start(cs::user_cmd* cmd) noexcept
{
    if (!config::get<bool>(vars::prediction))
        return;

    static bool once = []() {
        seed = *memory::find_bytes(dll::client, "8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04").offset(0x2).cast<int**>();
        player = *memory::find_bytes(dll::client, "89 35 ? ? ? ? F3 0F 10 48 20").offset(0x2).cast<cs::player***>();
        return true;
    }();

    update();

    *seed = 0;
    *player = static_cast<cs::player*>(local_player);

    backup.is_in_prediction = interfaces::prediction->is_in_prediction;
    backup.is_first_time_predicted = interfaces::prediction->is_first_time_predicted;
    backup.cur_time = interfaces::globals->cur_time;
    backup.frame_time = interfaces::globals->frame_time;
    backup.tick_count = interfaces::globals->tick_count;

    interfaces::globals->cur_time = math::ticks_to_time(local_player->get_tick_base());
    interfaces::globals->frame_time = interfaces::prediction->is_engine_paused ? 0.0f : interfaces::globals->interval_per_tick;
    interfaces::globals->tick_count = local_player->get_tick_base();
    interfaces::prediction->is_in_prediction = true;
    interfaces::prediction->is_first_time_predicted = false;

    if (cmd->weapon_select != 0) {
        if (const auto weapon = interfaces::entity_list->get(cmd->weapon_select))
            if (const auto info = weapon->get_weapon_info())
                local_player->select_item(info->weapon_name, cmd->weapon_subtype);
    }

    update_button_state(cmd->buttons);

    interfaces::prediction->check_moving_ground(local_player, interfaces::globals->frame_time);

    local_player->set_local_view_angles(cmd->view_angles);

    if (local_player->physics_run_think(cs::think_method::all_functions))
        local_player->pre_think();

    const int think_tick = local_player->get_next_think_tick();
    if (think_tick > 0 && think_tick <= local_player->get_tick_base()) {
        local_player->get_next_think_tick() = cs::tick_never_think;
        local_player->think();
    }

    interfaces::move_helper->set_host(local_player);

    interfaces::prediction->setup_move(local_player, cmd, interfaces::move_helper, &move_data);
    interfaces::movement->process_movement(local_player, &move_data);
    interfaces::prediction->finish_move(local_player, cmd, &move_data);

    interfaces::move_helper->process_impacts();

    se::mdl_cache_critical_section cs(interfaces::mdl_cache);

    if (local_player->is_alive()) {
        local_player->update_collision_bounds();

        if (local_player->get_flags().is_set(cs::entity_flag::on_ground))
            local_player->get_fall_velocity() = 0.0f;

        if (local_player->get_sequence() == -1)
            local_player->set_sequence(0);

        local_player->studio_frame_advance();
        local_player->post_think_v_physics();
    }

    local_player->simulate_player_simulated_entities();

    if (const auto weapon = local_player->get_active_weapon())
        weapon->update_accuracy_penalty();

    interfaces::prediction->is_in_prediction = backup.is_in_prediction;
    interfaces::prediction->is_first_time_predicted = backup.is_first_time_predicted;
}

void features::prediction::end() noexcept
{
    if (!config::get<bool>(vars::prediction))
        return;

    interfaces::move_helper->set_host(nullptr);

    interfaces::globals->cur_time = backup.cur_time;
    interfaces::globals->frame_time = backup.frame_time;
    interfaces::globals->tick_count = backup.tick_count;

    *seed = -1;
    *player = nullptr;

    interfaces::movement->reset();

    if (!interfaces::prediction->is_engine_paused && interfaces::globals->frame_time > 0.0f)
        local_player->get_tick_base()++;
}
