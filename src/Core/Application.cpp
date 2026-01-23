#include "Application.h"
#include <print>
#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "ApplicationLayout.h"
#include "ApplicationTheme.h"

extern "C" 
{
    #include "rlgl.h"
}

namespace Core 
{

    FApplication::FApplication(std::string_view InName, int InWidth, int InHeight)
        : Name(InName), 
          Width(InWidth), 
          Height(InHeight),
          WindowHandle(nullptr), 
          bIsRunning(false), 
          bRenderLoopFinished(false)
    {
    }

    FApplication::~FApplication()
    {
        if (RenderThread.joinable())
        {
            RenderThread.join();
        }
    }

    void FApplication::FramebufferSizeCallback
    (
        GLFWwindow* Window, 
        int InWidth, 
        int InHeight
    )
    {
        FApplication* App = (FApplication*)glfwGetWindowUserPointer(Window);
        if (App)
        {
            App->SetSize(InWidth, InHeight);
        }
    }

    void FApplication::WindowCloseCallback(GLFWwindow* Window)
    {
        FApplication* App = (FApplication*)glfwGetWindowUserPointer(Window);
        if (App)
        {
            App->bIsRunning = false; // Signal thread to stop
        }
    }

    void FApplication::Run()
    {
        if (!glfwInit())
        {
            std::println(stderr, "Failed to init GLFW");
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #if __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

        WindowHandle = glfwCreateWindow(Width, Height, Name.c_str(), nullptr, nullptr);
        if (!WindowHandle)
        {
            std::println(stderr, "Failed to create GLFW window");
            glfwTerminate();
            return;
        }

        // Setup Main Thread callbacks
        glfwSetWindowUserPointer(WindowHandle, this);
        glfwSetFramebufferSizeCallback(WindowHandle, FramebufferSizeCallback);
        glfwSetWindowCloseCallback(WindowHandle, WindowCloseCallback);

        // Initialize ImGui on Main Thread (Required for GLFW callbacks)
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        
        SetApplicationTheme();
        LoadApplicationDefaultIni();

        ImGui_ImplGlfw_InitForOpenGL(WindowHandle, true);

        // --- Thread Handoff ---
        // We MUST release the context from the Main Thread so the Render Thread can claim it.
        glfwMakeContextCurrent(nullptr);
        
        // Start Render Thread
        bIsRunning = true;
        RenderThread = std::thread(&FApplication::RenderLoop, this);

        // --- Main Event Loop ---
        // This loop handles OS events (Input, Window Move/Resize).
        // It runs completely independently of the Render Thread.
        // Even if this loop blocks (e.g. while dragging window), the Render Thread continues.
        while (bIsRunning)
        {
            // WaitEvents is more efficient for the main thread since we don't render here.
            // It sleeps until an input event arrives or a timeout.
            glfwWaitEvents();
            
            // Explicitly check for close to break the loop if bIsRunning was set to false by callback
            if (glfwWindowShouldClose(WindowHandle)) 
            {
                bIsRunning = false;
            }
        }

        // --- Cleanup ---
        // Wait for render thread to finish
        // We MUST keep pumping the event loop while waiting, otherwise 
        // the RenderThread might deadlock on SwapBuffers waiting for the main thread.
        while (!bRenderLoopFinished)
        {
            glfwWaitEventsTimeout(0.005); // Sleep 5ms but process events
        }

        if (RenderThread.joinable())
        {
            RenderThread.join();
        }

        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(WindowHandle);
        glfwTerminate();
    }
    
    // This runs on the SECONDARY THREAD
    void FApplication::RenderLoop()
    {
        // CLAIM CONTEXT on this thread
        glfwMakeContextCurrent(WindowHandle);
        glfwSwapInterval(1); // Disable VSync for max performance (or 1 if preferred)

        // Initialize GLAD / Extensions
        rlLoadExtensions((void*)glfwGetProcAddress);

        // ImGui Context created on Main Thread, just need IO here
        ImGuiIO& IO = ImGui::GetIO();
        
        // Init OpenGL3 Backend (Needs GL Context)
        ImGui_ImplOpenGL3_Init("#version 330");

        // Initialize Raylib's RLGL
        // Note: rlglInit uses internal bounds, we sync them via Width/Height
        rlglInit(Width, Height);

        // User Start
        OnStart();

        PreviousTime = glfwGetTime();

        while (bIsRunning)
        {
            double CurrentTime = glfwGetTime();
            float DeltaSeconds = static_cast<float>(CurrentTime - PreviousTime);
            PreviousTime = CurrentTime;

            // Update Viewport if resized (Atomic read)
            int CurrentW = Width;
            int CurrentH = Height;

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

            glViewport(0, 0, CurrentW, CurrentH);
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

            glfwSwapBuffers(WindowHandle);
        }

        // User Shutdown (on thread)
        OnShutdown();

        // Cleanup (on thread)
        rlglClose();
        ImGui_ImplOpenGL3_Shutdown();

        bRenderLoopFinished = true;
    }

}
