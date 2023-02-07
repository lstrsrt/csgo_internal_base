#pragma once

#include "../../crypt/crc32.h"

namespace cs {

enum class cmd_button {
    attack = (1 << 0),
    jump = (1 << 1),
    duck = (1 << 2),
    forward = (1 << 3),
    back = (1 << 4),
    use = (1 << 5),
    cancel = (1 << 6),
    left = (1 << 7),
    right = (1 << 8),
    move_left = (1 << 9),
    move_right = (1 << 10),
    second_attack = (1 << 11),
    run = (1 << 12),
    reload = (1 << 13),
    left_alt = (1 << 14),
    right_alt = (1 << 15),
    score = (1 << 16),
    speed = (1 << 17),
    walk = (1 << 18),
    zoom = (1 << 19),
    first_weapon = (1 << 20),
    second_weapon = (1 << 21),
    bullrush = (1 << 22),
    first_grenade = (1 << 23),
    second_grenade = (1 << 24),
    middle_attack = (1 << 25),
    use_or_reload = (1 << 26)
};

struct user_cmd {
    PAD(0x4)
    int number{ };
    int tick_count{ };
    angle view_angles{ };
    vec3 aim_direction{ };
    vec3 move_direction{ };
    bitfield<cmd_button> buttons{ };
    char impulse{ };
    int weapon_select{ };
    int weapon_subtype{ };
    int random_seed{ };
    short mouse_dx{ };
    short mouse_dy{ };
    bool has_been_predicted{ };
    PAD(0x18)

    inline crc32_t get_checksum() const noexcept
    {
        crc32_t ret{ std::numeric_limits<crc32_t>::max() };

        crc32::process_data(ret, &number);
        crc32::process_data(ret, &tick_count);
        crc32::process_data(ret, &view_angles);
        crc32::process_data(ret, &aim_direction);
        crc32::process_data(ret, &move_direction);
        crc32::process_data(ret, &buttons);
        crc32::process_data(ret, &impulse);
        crc32::process_data(ret, &weapon_select);
        crc32::process_data(ret, &weapon_subtype);
        crc32::process_data(ret, &random_seed);
        crc32::process_data(ret, &mouse_dx);
        crc32::process_data(ret, &mouse_dy);

        return ~ret;
    }
};

struct verified_user_cmd {
    user_cmd cmd{ };
    crc32_t crc{ };
};

struct move_data {
    bool first_run_of_functions : 1{ };
    bool game_code_moved_player : 1{ };
    bool no_air_control : 1{ };
    unsigned long player_handle{ };
    int impulse_cmd{ };
    angle view_angles{ };
    angle abs_view_angles{ };
    bitfield<cmd_button> buttons{ };
    bitfield<cmd_button> old_buttons{ };
    vec3 move_direction{ };
    float max_speed{ };
    float max_client_speed{ };
    vec3 velocity{ };
    vec3 trailing_velocity{ };
    float trailing_velocity_time{ };
    angle angles{ };
    angle old_angles{ };
    float out_step_height{ };
    vec3 out_wish_velocity{ };
    vec3 out_jump_velocity{ };
    vec3 constraint_center{ };
    float constraint_radius{ };
    float constraint_width{ };
    float constraint_speed_factor{ };
    bool constraint_past_radius{ };
    vec3 abs_origin{ };
};

}
