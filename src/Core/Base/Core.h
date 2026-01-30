#pragma once

#include <memory>

// Platform detection
#ifdef _WIN32
    #ifdef _WIN64
        #define CORE_PLATFORM_WINDOWS
    #else
        #error "x86 Builds are not supported!"
    #endif
#elif defined(__APPLE__) || defined(__MACH__)
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR == 1
        #error "IOS simulator not supported!"
    #elif TARGET_OS_IPHONE == 1
        #define CORE_PLATFORM_IOS
        #error "IOS not supported!"
    #elif TARGET_OS_MAC == 1
        #define CORE_PLATFORM_MACOS
    #else
        #error "Unknown Apple Platform!"
    #endif
#elif defined(__ANDROID__)
    #define CORE_PLATFORM_ANDROID
    #error "Android not supported!"
#elif defined(__linux__)
    #define CORE_PLATFORM_LINUX
#else
    #error "Unknown Platform!"
#endif

// Debug Assertions
#ifdef CORE_DEBUG
    #if defined(CORE_PLATFORM_WINDOWS)
        #define CORE_DEBUGBREAK() __debugbreak()
    #elif defined(CORE_PLATFORM_LINUX)
        #include <signal.h>
        #define CORE_DEBUGBREAK() raise(SIGTRAP)
    #else
        #define CORE_DEBUGBREAK()
    #endif

    #define CORE_ENABLE_ASSERTS
#else
    #define CORE_DEBUGBREAK()
#endif

#ifdef CORE_ENABLE_ASSERTS
    // Note: We use __VA_ARGS__ directly, but format string might need handling if user passes non-fmt friendly args?
    // The previous macro used __VA_ARGS__ as format args.
    #define CORE_ASSERT(x, ...) { if(!(x)) { ::Core::FLog::CoreError("Assertion Failed: {0}", __VA_ARGS__); CORE_DEBUGBREAK(); } }
#else
    #define CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define CORE_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Core {
    template<typename T>
    using Scope = std::unique_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}
