#pragma once

#include <filesystem>
#include <functional>
#include <Windows.h>
#include <TlHelp32.h>

struct dll_t;

namespace winapi {

    // Only usable for handles closed via CloseHandle()!
    struct scoped_handle {
        HANDLE handle{ };

        scoped_handle(HANDLE handle) noexcept
            : handle(handle) {};
        ~scoped_handle()
        {
            if (handle)
                CloseHandle(handle);
        }

        operator bool() noexcept
        {
            return handle != nullptr && handle != INVALID_HANDLE_VALUE;
        }

        operator HANDLE() noexcept
        {
            return handle;
        }
    };

    struct virtual_protect {
        LPVOID address{ };
        SIZE_T size{ };
        DWORD flags{ };

        explicit virtual_protect(LPVOID address, SIZE_T size, DWORD flags) noexcept
            : address(address), size(size)
        {
            VirtualProtect(address, size, flags, &flags);
        }
        explicit virtual_protect(LPVOID address, DWORD flags) noexcept
            : address(address), size(sizeof(address))
        {
            VirtualProtect(address, size, flags, &flags);
        }

        ~virtual_protect()
        {
            VirtualProtect(address, size, flags, &flags);
        }
    };

    inline bool get_module_path(HMODULE hmod, std::filesystem::path& path) noexcept
    {
        WCHAR tmp[MAX_PATH]{ };
        bool ret{ };
        if (ret = GetModuleFileNameW(hmod, tmp, MAX_PATH) != 0)
            path.assign(tmp);
        return ret;
    }

    inline void iterate_processes(std::function<void(PROCESSENTRY32W&)> callback) noexcept
    {
        scoped_handle snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32W entry{ sizeof(entry) };

        if (Process32FirstW(snapshot, &entry)) {
            while (Process32NextW(snapshot, &entry))
                callback(entry);
        }
    }

    inline DWORD get_process_id(std::wstring_view name) noexcept
    {
        DWORD ret{ };
        iterate_processes([&](const PROCESSENTRY32W& entry) {
            if (!name.compare(entry.szExeFile))
                ret = entry.th32ProcessID;
        });
        return ret;
    }

    inline scoped_handle get_process_handle(std::wstring_view name, DWORD access_flags) noexcept
    {
        return OpenProcess(access_flags, FALSE, get_process_id(name));
    }

}
