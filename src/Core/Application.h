#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include "imgui.h"

// Forward declaration to avoid including internal headers in the public API if possible, 
// strictly speaking user asked to use raylib methods, so including raylib.h here is good 
// so they don't have to include it manually in every file deriving from Application.
extern "C" {
#include "raylib.h"
}

struct GLFWwindow;

namespace Core {

    class Application {
    public:
        Application(const std::string& name = "App", int width = 1280, int height = 720);
        virtual ~Application();

        void Run();

        virtual void OnStart() {}
        virtual void OnUpdate(float dt) {}
        virtual void OnUIRender() {}
        virtual void OnShutdown() {}
        
        // Internal usage for thread
        void RenderLoop();
        GLFWwindow* GetWindow() const { return m_Window; }
        
        // Sync data
        int GetWidth() const { return m_Width; }
        int GetHeight() const { return m_Height; }
        void SetSize(int w, int h) { m_Width = w; m_Height = h; }

    private:
        static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
        static void WindowCloseCallback(GLFWwindow* window);

    private:
        std::string m_Name;
        std::atomic<int> m_Width;
        std::atomic<int> m_Height;
        GLFWwindow* m_Window;
        
        // Threading
        std::thread m_RenderThread;
        std::atomic<bool> m_Running;
        
        // Timing
        double m_PreviousTime = 0.0;
    };

}

// To be defined by the client
Core::Application* CreateApplication();
