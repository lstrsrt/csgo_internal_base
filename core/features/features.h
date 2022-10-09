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


    }

    namespace misc {


    }

}
