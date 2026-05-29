#pragma once

#include "Core/Base/Core.h" // IWYU pragma: keep
#include "Core/Events/Event.h"
#include <string>

namespace Core
{
    class FLayer
    {
    public:
        FLayer(const std::string& InName = "Layer");
        virtual ~FLayer();

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(float) {}
        virtual void OnUIRender() {}
        virtual void OnEvent(FEvent&) {}

        [[nodiscard]] const std::string& GetName() const { return DebugName; }
    protected:
        std::string DebugName;
    };
}
