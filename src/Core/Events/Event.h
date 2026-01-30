#pragma once

#include "Core/Base/Core.h"
#include <string>
#include <concepts>
#include <functional>
#include <sstream>

namespace Core {

    enum class EEventType
    {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        AppTick, AppUpdate, AppRender,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    enum EEventCategory
    {
        None = 0,
        EventCategoryApplication    = BIT(0),
        EventCategoryInput          = BIT(1),
        EventCategoryKeyboard       = BIT(2),
        EventCategoryMouse          = BIT(3),
        EventCategoryMouseButton    = BIT(4)
    };

#define EVENT_CLASS_TYPE(type) static EEventType GetStaticType() { return EEventType::type; }\
                               virtual EEventType GetEventType() const override { return GetStaticType(); }\
                               virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

    class FEvent
    {
    public:
        virtual ~FEvent() = default;

        bool bHandled = false;

        [[nodiscard]] virtual EEventType GetEventType() const = 0;
        [[nodiscard]] virtual const char* GetName() const = 0;
        [[nodiscard]] virtual int GetCategoryFlags() const = 0;
        [[nodiscard]] virtual std::string ToString() const { return GetName(); }

        [[nodiscard]] bool IsInCategory(EEventCategory InCategory) const
        {
            return GetCategoryFlags() & InCategory;
        }
    };

    class FEventDispatcher
    {
    public:
        FEventDispatcher(FEvent& InEvent)
            : Event(InEvent)
        {
        }

        // Concept to ensure T is a valid Event type
        template<typename T, typename F>
        requires std::derived_from<T, FEvent>
        bool Dispatch(const F& Func)
        {
            if (Event.GetEventType() == T::GetStaticType())
            {
                Event.bHandled |= Func(static_cast<T&>(Event));
                return true;
            }
            return false;
        }
    private:
        FEvent& Event;
    };

    inline std::ostream& operator<<(std::ostream& OS, const FEvent& E)
    {
        return OS << E.ToString();
    }

}
