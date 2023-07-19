#pragma once

#if __has_include(<stacktrace>) && _HAS_CXX23
#define HAS_STACKTRACE
#include <stacktrace>
#endif

#include "logger.h"

#ifdef HAS_STACKTRACE
static void print_trace() noexcept
{
    auto trace = std::stacktrace::current(2);
    LOG_RAW("Call trace: ");
    for (const auto& entry : trace)
        LOG_RAW("{}", entry.description());
}
#endif

#ifdef NDEBUG
[[noreturn]]
#endif
static void fail(std::string_view fn, std::string_view msg = "") noexcept
{
#ifdef _DEBUG
    if (msg.empty())
        LOG_ERROR("An error occurred in {}!", fn);
    else
        LOG_ERROR("{} ({})", msg, fn);
#ifdef HAS_STACKTRACE
    print_trace();
#endif
    __debugbreak();
#else
    std::abort();
#endif
}

#ifdef __clang__
#define FUNCTION_NAME __PRETTY_FUNCTION__
#else
#define FUNCTION_NAME __FUNCTION__
#endif
#define VERIFY(expr) do { if (!(expr)) fail(FUNCTION_NAME); } while (false)
#define VERIFY_MSG(expr, msg) do { if (!(expr)) fail(FUNCTION_NAME, msg); } while (false)
