#include "../hooks.h"
#include "../netvars.h"
#include "../features/variables.h"

void hooks::spotted::proxy(cs::recv_proxy_data* data, void* arg0, void* arg1)
{
    if (cfg::get<bool>(vars::radar_reveal))
        data->value.integer = 1;

    return original(data, arg0, arg1);
}
