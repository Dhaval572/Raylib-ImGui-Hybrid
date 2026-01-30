#pragma once

#include "Core/Base/Core.h"

#include <memory>
#include <string>
#include <format>
#include <print>
#include <iostream>

namespace Core {

    class FLog
    {
    public:
        enum class ELogLevel
        {
            Debug = 0, Warn, Error
        };

    public:
        template<typename... Args>
        static void PrintMessage(ELogLevel Level, std::string_view Tag, std::format_string<Args...> Fmt, Args&&... args)
        {
            std::string Message = std::format(Fmt, std::forward<Args>(args)...);
            PrintInternal(Level, Tag, Message);
        }

        // Template API
        template<typename... Args>
        static void Debug(std::format_string<Args...> Fmt, Args&&... args)
        {
            PrintMessage(ELogLevel::Debug, "APP", Fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void Warn(std::format_string<Args...> Fmt, Args&&... args)
        {
            PrintMessage(ELogLevel::Warn, "APP", Fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void Error(std::format_string<Args...> Fmt, Args&&... args)
        {
            PrintMessage(ELogLevel::Error, "APP", Fmt, std::forward<Args>(args)...);
        }

        // Core variants
        template<typename... Args>
        static void CoreDebug(std::format_string<Args...> Fmt, Args&&... args)
        {
            PrintMessage(ELogLevel::Debug, "CORE", Fmt, std::forward<Args>(args)...);
        }
        
        template<typename... Args>
        static void CoreWarn(std::format_string<Args...> Fmt, Args&&... args)
        {
            PrintMessage(ELogLevel::Warn, "CORE", Fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void CoreError(std::format_string<Args...> Fmt, Args&&... args)
        {
            PrintMessage(ELogLevel::Error, "CORE", Fmt, std::forward<Args>(args)...);
        }

    private:
        static void PrintInternal(ELogLevel Level, std::string_view Tag, std::string_view Message);
    };

}
