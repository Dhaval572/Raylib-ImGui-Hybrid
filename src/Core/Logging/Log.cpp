#include "Log.h"
#include <mutex>

namespace Core {

    static std::mutex s_LogMutex;

    void FLog::PrintInternal(ELogLevel Level, std::string_view Tag, std::string_view Message)
    {
        std::lock_guard<std::mutex> Lock(s_LogMutex);

        // ANSI Color Codes
        const char* ColorCode = "\033[0m"; // Reset
        const char* LevelStr = "INFO";

        switch (Level)
        {
            case ELogLevel::Debug: ColorCode = "\033[36m"; LevelStr = "DEBUG"; break; // Cyan
            case ELogLevel::Warn:  ColorCode = "\033[33m"; LevelStr = "WARN "; break; // Yellow
            case ELogLevel::Error: ColorCode = "\033[31m"; LevelStr = "ERROR"; break; // Red
        }

        std::println(stdout, "{}[{}] {}: {}{}", ColorCode, Tag, LevelStr, Message, "\033[0m");
    }

}
