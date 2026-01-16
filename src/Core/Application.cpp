#include "Application.h"
#include <iostream>
#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

extern "C" {
    #include "rlgl.h"
}

namespace Core {

    Application::Application(const std::string& name, int width, int height)
        : m_Name(name), m_Width(width), m_Height(height), m_Window(nullptr), m_Running(false)
    {
    }

    Application::~Application()
    {
        // Join thread if still joinable
        if (m_RenderThread.joinable())
            m_RenderThread.join();
    }

    void Application::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        Application* app = (Application*)glfwGetWindowUserPointer(window);
        if (app)
        {
            app->SetSize(width, height);
        }
    }

    void Application::WindowCloseCallback(GLFWwindow* window)
    {
        Application* app = (Application*)glfwGetWindowUserPointer(window);
        if (app)
        {
            app->m_Running = false; // Signal thread to stop
        }
    }

    void Application::Run()
    {
        if (!glfwInit())
        {
            std::cerr << "Failed to init GLFW\n";
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #if __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

        m_Window = glfwCreateWindow(m_Width, m_Height, m_Name.c_str(), nullptr, nullptr);
        if (!m_Window)
        {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            return;
        }

        // Setup Main Thread callbacks
        glfwSetWindowUserPointer(m_Window, this);
        glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);
        glfwSetWindowCloseCallback(m_Window, WindowCloseCallback);

        // --- Thread Handoff ---
        // We MUST release the context from the Main Thread so the Render Thread can claim it.
        glfwMakeContextCurrent(NULL);
        
        // Start Render Thread
        m_Running = true;
        m_RenderThread = std::thread(&Application::RenderLoop, this);

        // --- Main Event Loop ---
        // This loop handles OS events (Input, Window Move/Resize).
        // It runs completely independently of the Render Thread.
        // Even if this loop blocks (e.g. while dragging window), the Render Thread continues.
        while (m_Running)
        {
            // WaitEvents is more efficient for the main thread since we don't render here.
            // It sleeps until an input event arrives or a timeout.
            glfwWaitEvents();
            
            // Explicitly check for close to break the loop if m_Running was set to false by callback
            if (glfwWindowShouldClose(m_Window)) 
                m_Running = false;
        }

        // --- Cleanup ---
        // Wait for render thread to finish
        if (m_RenderThread.joinable())
            m_RenderThread.join();

        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }
    
    // This runs on the SECONDARY THREAD
    void Application::RenderLoop()
    {
        // CLAIM CONTEXT on this thread
        glfwMakeContextCurrent(m_Window);
        glfwSwapInterval(0); // Disable VSync for max performance (or 1 if preferred)

        // Initialize GLAD / Extensions
        rlLoadExtensions((void*)glfwGetProcAddress);

        // Initialize ImGui (Needs to happen on the thread with context)
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& IO = ImGui::GetIO(); (void)IO;
        IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        // Initialize Raylib's RLGL
        // Note: rlglInit uses internal bounds, we sync them via m_Width/m_Height
        rlglInit(m_Width, m_Height);

        // User Start
        OnStart();

        m_PreviousTime = glfwGetTime();

        while (m_Running)
        {
            double CurrentTime = glfwGetTime();
            float DeltaSeconds = (float)(CurrentTime - m_PreviousTime);
            m_PreviousTime = CurrentTime;

            // Update Viewport if resized (Atomic read)
            int currentW = m_Width;
            int currentH = m_Height;

            // User Update
            OnUpdate(DeltaSeconds);

            // ImGui Frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // User UI Render
            OnUIRender();

            // Render Frame
            ImGui::Render();

            glViewport(0, 0, currentW, currentH);
            glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow* BackupCurrentContext = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(BackupCurrentContext);
            }

            glfwSwapBuffers(m_Window);
        }

        // User Shutdown (on thread)
        OnShutdown();

        // Cleanup (on thread)
        rlglClose();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

}
