#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include "imgui.h"

#include "Core/Events/Event.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Layers/LayerStack.h"
#include "Core/Application/ApplicationConfig.h"

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
        FApplication(const FApplicationConfig& InConfig = FApplicationConfig());
        virtual ~FApplication();
        static FApplication& Get() { return *s_Instance; }

        void Run();
        void OnEvent(FEvent& InEvent);

        void PushLayer(FLayer* InLayer);
        void PushOverlay(FLayer* InLayer);

        // Legacy Virtuals
        virtual void OnStart() {}
        virtual void OnUpdate(float DeltaTime) {}
        virtual void OnUIRender() {}
        virtual void OnShutdown() {}
        
        // Internal usage for thread
        void RenderLoop();
        [[nodiscard]] GLFWwindow* GetWindow() const { return WindowHandle; }
        
        // Sync data
        [[nodiscard]] int GetWidth() const { return Width; }
        [[nodiscard]] int GetHeight() const { return Height; }
        void SetSize(int NewWidth, int NewHeight) { Width = NewWidth; Height = NewHeight; }

    private:
        static void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height);
        static void WindowCloseCallback(GLFWwindow* Window);

        bool OnWindowClose(FWindowCloseEvent& e);
        bool OnWindowResize(FWindowResizeEvent& e);

    private:
        std::string Name;
        FApplicationConfig Config;
        
        std::atomic<int> Width;
        std::atomic<int> Height;
        GLFWwindow* WindowHandle;
        
        FLayerStack LayerStack;

        // Threading
        std::thread RenderThread;
        std::atomic<bool> bIsRunning;
        std::atomic<bool> bRenderLoopFinished;
        
        // Timing
        double PreviousTime = 0.0;
    
    private:
        static FApplication* s_Instance;
    };
}
