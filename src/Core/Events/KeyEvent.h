#pragma once

#include "Event.h"
#include <sstream>

namespace Core {

    class FKeyEvent : public FEvent
    {
    public:
        [[nodiscard]] int GetKeyCode() const { return KeyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard)
    protected:
        FKeyEvent(int InKeyCode)
            : KeyCode(InKeyCode) {}

        int KeyCode;
    };

    class FKeyPressedEvent : public FKeyEvent
    {
    public:
        FKeyPressedEvent(int InKeyCode, bool bInIsRepeat = false)
            : FKeyEvent(InKeyCode), bIsRepeat(bInIsRepeat) {}

        [[nodiscard]] bool IsRepeat() const { return bIsRepeat; }

        [[nodiscard]] std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << KeyCode << " (repeat = " << bIsRepeat << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPressed)

    private:
        bool bIsRepeat;
    };

    class FKeyReleasedEvent : public FKeyEvent
    {
    public:
        FKeyReleasedEvent(int InKeyCode)
            : FKeyEvent(InKeyCode) {}

        [[nodiscard]] std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class FKeyTypedEvent : public FKeyEvent
    {
    public:
        FKeyTypedEvent(int InKeyCode)
            : FKeyEvent(InKeyCode) {}

        [[nodiscard]] std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyTypedEvent: " << KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyTyped)
    };
}
