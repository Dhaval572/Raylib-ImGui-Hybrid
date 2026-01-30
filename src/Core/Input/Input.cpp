#include "Input.h"
#include "Core/Application/Application.h"
#include <GLFW/glfw3.h>

namespace Core {

    bool FInput::IsKeyPressed(int KeyCode)
    {
        auto Window = static_cast<GLFWwindow*>(FApplication::Get().GetWindow());
        auto State = glfwGetKey(Window, KeyCode);
        return State == GLFW_PRESS || State == GLFW_REPEAT;
    }

    bool FInput::IsMouseButtonPressed(int Button)
    {
        auto Window = static_cast<GLFWwindow*>(FApplication::Get().GetWindow());
        auto State = glfwGetMouseButton(Window, Button);
        return State == GLFW_PRESS;
    }

    std::pair<float, float> FInput::GetMousePosition()
    {
        auto Window = static_cast<GLFWwindow*>(FApplication::Get().GetWindow());
        double XPos, YPos;
        glfwGetCursorPos(Window, &XPos, &YPos);
        return { (float)XPos, (float)YPos };
    }

    float FInput::GetMouseX()
    {
        auto [X, Y] = GetMousePosition();
        return X;
    }

    float FInput::GetMouseY()
    {
        auto [X, Y] = GetMousePosition();
        return Y;
    }

}
