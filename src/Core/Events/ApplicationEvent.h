#pragma once

#include "Event.h"
#include <sstream>

namespace Core {

    class FWindowResizeEvent : public FEvent
    {
    public:
        FWindowResizeEvent(unsigned int InWidth, unsigned int InHeight)
            : Width(InWidth), Height(InHeight) {}

        [[nodiscard]] unsigned int GetWidth() const { return Width; }
        [[nodiscard]] unsigned int GetHeight() const { return Height; }

        [[nodiscard]] std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << Width << ", " << Height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
        unsigned int Width, Height;
    };

    class FWindowCloseEvent : public FEvent
    {
    public:
        FWindowCloseEvent() = default;

        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class FAppTickEvent : public FEvent
    {
    public:
        FAppTickEvent() = default;

        EVENT_CLASS_TYPE(AppTick)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class FAppUpdateEvent : public FEvent
    {
    public:
        FAppUpdateEvent() = default;

        EVENT_CLASS_TYPE(AppUpdate)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class FAppRenderEvent : public FEvent
    {
    public:
        FAppRenderEvent() = default;

        EVENT_CLASS_TYPE(AppRender)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };
}
