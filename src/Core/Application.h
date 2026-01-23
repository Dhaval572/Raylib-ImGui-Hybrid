#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include "imgui.h"

// Forward declaration to avoid including internal headers in the public API if possible, 
// strictly speaking user asked to use raylib methods, so including raylib.h here is good 
// so they don't have to include it manually in every file deriving from Application.
extern "C" 
{
    #include "raylib.h" 
}

struct GLFWwindow;

namespace Core 
{

    class FApplication 
    {
    public:
        FApplication(std::string_view Name = "App", int Width = 1280, int Height = 720);
        virtual ~FApplication();

        void Run();

        virtual void OnStart() {}
        virtual void OnUpdate(float DeltaTime) {}
        virtual void OnUIRender() {}
        virtual void OnShutdown() {}
        
        // Internal usage for thread
        void RenderLoop();
        GLFWwindow* GetWindow() const { return WindowHandle; }
        
        // Sync data
        int GetWidth() const { return Width; }
        int GetHeight() const { return Height; }
        void SetSize(int NewWidth, int NewHeight) { Width = NewWidth; Height = NewHeight; }

    private:
        static void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height);
        static void WindowCloseCallback(GLFWwindow* Window);

    private:
        std::string Name;
        std::atomic<int> Width;
        std::atomic<int> Height;
        GLFWwindow* WindowHandle;
        
        // Threading
        std::thread RenderThread;
        std::atomic<bool> bIsRunning;
        std::atomic<bool> bRenderLoopFinished;
        
        // Timing
        double PreviousTime = 0.0;
    };
}
