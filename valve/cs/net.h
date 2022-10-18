#pragma once

namespace se { struct net_channel; }

namespace cs {

enum class flow_direction {
    outgoing, incoming
};

enum class netaddr_type {
    null,
    loopback,
    broadcast,
    ip
};

struct netaddr {
    netaddr_type type{ };
    unsigned char ip[4]{ };
    unsigned short port{ };
};

struct server_info {
    netaddr net_addr{ };
    netaddr net_addr_online{ };
    uint16_t port{ };

    bool active{ };
    bool dedicated{ };
    bool lobby_exclusive{ };
    bool group_exclusive{ };
    bool in_main_menu_background{ };

    char const* server_name{ };
    char const* map_name{ };
    char const* map_group_name{ };

    int max_human_players{ };
    int human_players{ };
};

struct client_info {
    int signon_state{ };
    int socket{ };
    se::net_channel* net_channel{ };
    int human_players{ };
};

enum class clc_msg {
    file_crc_check = 14
};

enum class message_group {
    voice = 9
};

}

namespace se {

struct net_channel_info {
    VIRTUAL_FUNCTION(get_name, const char*, 0, (), (this))
    VIRTUAL_FUNCTION(get_address, const char*, 1, (), (this))
    VIRTUAL_FUNCTION(get_latency, float, 9, (cs::flow_direction flow), (this, flow))
    VIRTUAL_FUNCTION(get_avg_latency, float, 10, (cs::flow_direction flow), (this, flow))
    VIRTUAL_FUNCTION(get_avg_loss, float, 11, (cs::flow_direction flow), (this, flow))
    VIRTUAL_FUNCTION(get_avg_choke, float, 12, (cs::flow_direction flow), (this, flow))
    VIRTUAL_FUNCTION(get_avg_data, float, 13, (cs::flow_direction flow), (this, flow))
    VIRTUAL_FUNCTION(get_avg_packets, float, 14, (cs::flow_direction flow), (this, flow))
    VIRTUAL_FUNCTION(get_sequence_number, int, 17, (cs::flow_direction flow), (this, flow))
    VIRTUAL_FUNCTION(is_valid_packet, bool, 18, (cs::flow_direction flow, int frame_number), (this, flow, frame_number))
};

struct net_message {
    VIRTUAL_FUNCTION(get_type, cs::clc_msg, 7, (), (this))
    VIRTUAL_FUNCTION(get_group, cs::message_group, 8, (), (this))
};

struct net_channel : public net_channel_info {
    PAD(0x14)
    bool processing_messages{ };
    bool should_delete{ };
    PAD(0x2)
    int	out_sequence_number{ };
    int	in_sequence_number{ };
    int out_sequence_number_acknowledged{ };
    int out_reliable_state{ };
    int in_reliable_state{ };
    int choked_packets{ };
};

struct net_support {
    VIRTUAL_FUNCTION(get_server_info, void, 2, (cs::server_info* info), (this, info))
    VIRTUAL_FUNCTION(get_client_info, void, 3, (cs::client_info* info), (this, info))
};

}
