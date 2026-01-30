#pragma once

#include "Core/Base/Core.h"
#include "Core/Events/Event.h"
#include <string>

namespace Core {

    class FLayer
    {
    public:
        FLayer(const std::string& InName = "Layer");
        virtual ~FLayer();

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(float DeltaTime) {}
        virtual void OnUIRender() {}
        virtual void OnEvent(FEvent& InEvent) {}

        [[nodiscard]] const std::string& GetName() const { return DebugName; }
    protected:
        std::string DebugName;
    };

}
