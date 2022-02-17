#pragma once

#include <chrono>
#include <format>
#include <fstream>
#include <iostream>

#include "winapi.h"

#ifdef _DEBUG
#define LOG_SUCCESS(fmt, ...) logger::add(logger::level::success, fmt, __VA_ARGS__)
#define LOG_INFO(fmt, ...) logger::add(logger::level::info, fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...) logger::add(logger::level::error, fmt, __VA_ARGS__)
#else
#define LOG_SUCCESS
#define LOG_ERROR
#define LOG_INFO
#endif

namespace logger {

    inline HANDLE console_handle{ };
    inline std::wstring log_name{ };
    
    enum class level
    {
        success,
        info,
        error
    };
    
    // Use the LOG macros instead of accessing these directly

    template<class... va_args>
    inline void add(level lvl, std::string_view fmt, const va_args&... args) noexcept
    {
    #ifdef _DEBUG
        const auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        const auto msg = std::format(fmt, args...);
        
        std::cout << "[ ";
        switch (lvl) {
        case level::success:
            SetConsoleTextAttribute(console_handle, FOREGROUND_GREEN);
            std::cout << '+';
            break;
        case level::info:
            SetConsoleTextAttribute(console_handle, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            std::cout << '*';
            break;
        case level::error:
            SetConsoleTextAttribute(console_handle, FOREGROUND_RED);
            std::cout << '!';
            break;
        }

        SetConsoleTextAttribute(console_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        std::cout << " ] " << std::put_time(std::localtime(&time), "[%T] ");
        std::cout << msg << std::endl;
        
        std::fstream out{ log_name, std::fstream::out | std::fstream::app };
        if (out.good()) {
            out << msg << '\n';
            out.flush();
        }
    #endif
    }
    
    template<class... va_args>
    inline void add(level lvl, std::wstring_view fmt, const va_args&... args) noexcept
    {
    #ifdef _DEBUG
        const auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        const auto msg = std::format(fmt, args...);
        
        std::wcout << L"[ ";
        switch (lvl) {
        case level::success:
            SetConsoleTextAttribute(console_handle, FOREGROUND_GREEN);
            std::wcout << L'+';
            break;
        case level::info:
            SetConsoleTextAttribute(console_handle, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            std::wcout << L'*';
            break;
        case level::error:
            SetConsoleTextAttribute(console_handle, FOREGROUND_RED);
            std::wcout << L'!';
            break;
        }

        SetConsoleTextAttribute(console_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        std::wcout << L" ] " << std::put_time(std::localtime(&time), L"[%T] ");
        std::wcout << msg << std::endl;
        
        std::wfstream out{ log_name, std::wfstream::out | std::wfstream::app };
        if (out.good()) {
            out << msg << '\n';
            out.flush();
        }
    #endif
    }
    
    inline void initialize(std::wstring_view console_title, std::wstring_view log_filename) noexcept
    {
    #ifdef _DEBUG
        AllocConsole();
        AttachConsole(ATTACH_PARENT_PROCESS);
        SetConsoleTitleW(console_title.data());

        freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
        console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
        
        log_name = log_filename;
        winapi::scoped_handle f = CreateFileW(log_name.c_str(), 0, 0, nullptr, CREATE_NEW, 0, nullptr);

        add(level::info, "Initialized logger.");
    #endif
    }
    
    inline void end() noexcept
    {
    #ifdef _DEBUG
        fclose(stdout);
        FreeConsole();
    #endif
}

}
