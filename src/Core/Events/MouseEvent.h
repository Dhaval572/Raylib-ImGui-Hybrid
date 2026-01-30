#pragma once

#include "Event.h"
#include <sstream>

namespace Core {

    class FMouseMovedEvent : public FEvent
    {
    public:
        FMouseMovedEvent(float InX, float InY)
            : MouseX(InX), MouseY(InY) {}

        [[nodiscard]] float GetX() const { return MouseX; }
        [[nodiscard]] float GetY() const { return MouseY; }

        [[nodiscard]] std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseMovedEvent: " << MouseX << ", " << MouseY;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseMoved)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    private:
        float MouseX, MouseY;
    };

    class FMouseScrolledEvent : public FEvent
    {
    public:
        FMouseScrolledEvent(float InXOffset, float InYOffset)
            : XOffset(InXOffset), YOffset(InYOffset) {}

        [[nodiscard]] float GetXOffset() const { return XOffset; }
        [[nodiscard]] float GetYOffset() const { return YOffset; }

        [[nodiscard]] std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseScrolledEvent: " << XOffset << ", " << YOffset;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseScrolled)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    private:
        float XOffset, YOffset;
    };

    class FMouseButtonEvent : public FEvent
    {
    public:
        [[nodiscard]] int GetMouseButton() const { return Button; }

        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    protected:
        FMouseButtonEvent(int InButton)
            : Button(InButton) {}

        int Button;
    };

    class FMouseButtonPressedEvent : public FMouseButtonEvent
    {
    public:
        FMouseButtonPressedEvent(int InButton)
            : FMouseButtonEvent(InButton) {}

        [[nodiscard]] std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonPressedEvent: " << Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonPressed)
    };

    class FMouseButtonReleasedEvent : public FMouseButtonEvent
    {
    public:
        FMouseButtonReleasedEvent(int InButton)
            : FMouseButtonEvent(InButton) {}

        [[nodiscard]] std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonReleasedEvent: " << Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonReleased)
    };

}
