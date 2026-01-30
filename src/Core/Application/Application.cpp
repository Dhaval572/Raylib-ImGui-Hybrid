#include "Application.h"
#include "Core/Logging/Log.h"
#include "Core/Input/Input.h"

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

    FApplication* FApplication::s_Instance = nullptr;

    FApplication::FApplication(const FApplicationConfig& InConfig)
        : Config(InConfig),
          Name(InConfig.Name), 
          Width(InConfig.Width), 
          Height(InConfig.Height),
          WindowHandle(nullptr), 
          bIsRunning(false), 
          bRenderLoopFinished(false)
    {
        CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;
    }

    FApplication::~FApplication()
    {
        if (RenderThread.joinable())
        {
            RenderThread.join();
        }
    }

    void FApplication::PushLayer(FLayer* InLayer)
    {
        LayerStack.PushLayer(InLayer);
    }

    void FApplication::PushOverlay(FLayer* InOverlay)
    {
        LayerStack.PushOverlay(InOverlay);
    }

    void FApplication::OnEvent(FEvent& InEvent)
    {
        FEventDispatcher Dispatcher(InEvent);
        Dispatcher.Dispatch<FWindowCloseEvent>(CORE_BIND_EVENT_FN(FApplication::OnWindowClose));
        Dispatcher.Dispatch<FWindowResizeEvent>(CORE_BIND_EVENT_FN(FApplication::OnWindowResize));

        for (auto It = LayerStack.rbegin(); It != LayerStack.rend(); ++It)
        {
            if (InEvent.bHandled) 
                break;
            (*It)->OnEvent(InEvent);
        }
    }

    bool FApplication::OnWindowClose(FWindowCloseEvent& e)
    {
        bIsRunning = false;
        return true;
    }

    bool FApplication::OnWindowResize(FWindowResizeEvent& e)
    {
        if (e.GetWidth() == 0 || e.GetHeight() == 0)
        {
            return false;
        }

        SetSize(e.GetWidth(), e.GetHeight());
        return false;
    }

    void FApplication::FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height)
    {
        FApplication* App = (FApplication*)glfwGetWindowUserPointer(Window);
        if (App)
        {
            FWindowResizeEvent Event(Width, Height);
            App->OnEvent(Event);
        }
    }

    void FApplication::WindowCloseCallback(GLFWwindow* Window)
    {
        FApplication* App = (FApplication*)glfwGetWindowUserPointer(Window);
        if (App)
        {
            FWindowCloseEvent Event;
            App->OnEvent(Event);
        }
    }

    void FApplication::Run()
    {
        if (!glfwInit())
        {
            FLog::CoreError("Failed to init GLFW");
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
            FLog::CoreError("Failed to create GLFW window");
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
        while (bIsRunning)
        {
            glfwWaitEvents();
            
            // Explicitly check for close to break the loop if bIsRunning was set to false by callback
            if (glfwWindowShouldClose(WindowHandle)) 
            {
                bIsRunning = false;
            }
        }

        // --- Cleanup ---
        while (!bRenderLoopFinished)
        {
            glfwWaitEventsTimeout(0.005);
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
        glfwSwapInterval(1); // Disable VSync for max performance (or 1 if preferred at 60)

        // Initialize GLAD / Extensions
        rlLoadExtensions((void*)glfwGetProcAddress);

        // ImGui Context created on Main Thread, just need IO here
        ImGuiIO& IO = ImGui::GetIO();
        
        // Init OpenGL3 Backend (Needs GL Context)
        ImGui_ImplOpenGL3_Init("#version 330");

        // Initialize Raylib's RLGL
        rlglInit(Width, Height);

        // User Start
        OnStart();

        PreviousTime = glfwGetTime();

        while (bIsRunning)
        {
            double CurrentTime = glfwGetTime();
            float DeltaSeconds = static_cast<float>(CurrentTime - PreviousTime);
            PreviousTime = CurrentTime;

            // Updated Local Width/Height
            int CurrentW = Width;
            int CurrentH = Height;

            // Update Layers
            for (FLayer* Layer : LayerStack)
                Layer->OnUpdate(DeltaSeconds);

            // Legacy Update (Keep for now)
            OnUpdate(DeltaSeconds);

            // ImGui Frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Render Layers UI
            for (FLayer* Layer : LayerStack)
                Layer->OnUIRender();

            // Legacy UI Render
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
