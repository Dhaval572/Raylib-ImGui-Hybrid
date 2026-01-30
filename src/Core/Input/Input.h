#pragma once

#include <utility>

namespace Core {

    class FInput
    {
    public:
        [[nodiscard]] static bool IsKeyPressed(int KeyCode);
        [[nodiscard]] static bool IsMouseButtonPressed(int Button);
        [[nodiscard]] static std::pair<float, float> GetMousePosition();
        [[nodiscard]] static float GetMouseX();
        [[nodiscard]] static float GetMouseY();
    };

}
