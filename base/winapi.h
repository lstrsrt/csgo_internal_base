#pragma once

#include <filesystem>
namespace fs = std::filesystem;
#include <string_view>
#include <Windows.h>
#include <TlHelp32.h>

struct dll;

namespace win {

    // Only usable for handles closed via CloseHandle()!
    struct scoped_handle {
        HANDLE handle{ };

        scoped_handle(HANDLE handle) noexcept
            : handle(handle) { }
        ~scoped_handle()
        {
            if (is_valid())
                CloseHandle(handle);
        }

        operator bool() noexcept { return is_valid(); }
        operator HANDLE() noexcept { return handle; }

        bool is_valid() const noexcept
        {
            return handle != nullptr && handle != INVALID_HANDLE_VALUE;
        }
    };

    inline bool get_module_path(HMODULE mod, fs::path& path) noexcept
    {
        WCHAR tmp[MAX_PATH]{ };
        bool ret{ };
        if ((ret = GetModuleFileNameW(mod, tmp, MAX_PATH)) != 0)
            path.assign(tmp);
        return ret;
    }

    inline void iterate_processes(std::invocable<PROCESSENTRY32W&> auto&& callback) noexcept
    {
        scoped_handle snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32W entry{ sizeof(entry) };

        if (Process32FirstW(snapshot, &entry)) {
            do
                callback(entry);
            while (Process32NextW(snapshot, &entry));
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
