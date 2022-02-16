#include "cheat.h"

static DWORD WINAPI on_attach(LPVOID);

BOOL APIENTRY DllMain(HMODULE instance, DWORD call_reason, LPVOID reserved)
{
    if (call_reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(instance);
        std::thread{ on_attach, instance }.detach();
        return TRUE;
    }
}

static DWORD WINAPI on_attach(LPVOID instance)
{
    cheat::initialize();

    while (!cheat::should_detach)
        std::this_thread::sleep_for(100ms);

    cheat::end();
    return 0;
}
