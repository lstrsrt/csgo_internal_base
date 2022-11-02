#pragma once

#include "../../base/base.h"
#include "../../valve/cs/cs.h"
#include "../../memory/interfaces.h"
#include "../cheat.h"
#include "variables.h"

inline namespace features {

    using cheat::local;

    namespace prediction {

        struct {
            bool is_in_prediction{ };
            bool is_first_time_predicted{ };
            float cur_time{ };
            float frame_time{ };
            int tick_count{ };
        } backup;
        inline int* seed{ };
        inline cs::player** player{ };
        inline cs::move_data move_data{ };

        void start(cs::user_cmd* cmd) noexcept;
        void end() noexcept;

    }

    namespace visuals {

        namespace esp {

            struct {
                int left{ };
                int top{ };
                int right{ };
                int bottom{ };
                int width{ };
                int height{ };
                uint8_t alpha{ };
            } inline box;

            void run() noexcept;
            void draw_box() noexcept;
            void draw_name(std::string_view name) noexcept;
            void draw_weapon(cs::weapon* weapon) noexcept;
            void draw_health(int health) noexcept;
            bool generate_box(cs::base_entity* entity, bool is_player) noexcept;

        }

    }

    namespace misc {


    }

}
