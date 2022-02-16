#pragma once

namespace cs { struct key_values; }

namespace se {

struct match_session;

struct match_framework {
    VIRTUAL_FUNCTION(get_match_session, match_session*, 13, (), (this))
    VIRTUAL_FUNCTION(create_session, void, 16, (cs::key_values* settings), (this, settings))
    VIRTUAL_FUNCTION(accept_invite, void, 18, (int controller), (this, controller))
    VIRTUAL_FUNCTION(close_session, void, 19, (), (this))
};

struct match_session {
    VIRTUAL_FUNCTION(get_session_system_data, cs::key_values*, 0, (), (this))
    VIRTUAL_FUNCTION(get_session_settings, cs::key_values*, 1, (), (this))
    VIRTUAL_FUNCTION(update_session_settings, void, 2, (cs::key_values* settings), (this, settings))
    VIRTUAL_FUNCTION(command, void, 3, (cs::key_values* cmd), (this, cmd))
};

}
