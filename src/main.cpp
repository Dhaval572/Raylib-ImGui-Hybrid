// main.cpp
// Hybrid: GLFW (window) + ImGui (UI & loop) + raylib (offscreen renderer)

#include <iostream>
#include <cstdlib>

#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

extern "C"
{
    #include "raylib.h"
    #include "rlgl.h"
}

static RenderTexture2D CreateRenderTexture(int Width, int Height)
{
    RenderTexture2D RenderTex = LoadRenderTexture(Width, Height);
    return RenderTex;
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to init GLFW\n";
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    const int WindowWidth = 1280;
    const int WindowHeight = 720;
    GLFWwindow* Window = glfwCreateWindow(WindowWidth, WindowHeight, "ImGui + raylib (offscreen)", nullptr, nullptr);
    if (!Window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(Window);
    glfwSwapInterval(1);

    rlLoadExtensions((void*)glfwGetProcAddress);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& IO = ImGui::GetIO(); (void)IO;
    IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(Window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    rlglInit(WindowWidth, WindowHeight);

    int ViewportTexW = 1280;
    int ViewportTexH = 720;
    RenderTexture2D SceneTexture = CreateRenderTexture(ViewportTexW, ViewportTexH);

    Camera2D Camera = { 0 };
    Camera.target = Vector2{ 0.0f, 0.0f };
    Camera.offset = Vector2{ ViewportTexW * 0.5f, ViewportTexH * 0.5f };
    Camera.zoom = 1.0f;

    // Timing
    double PreviousTime = glfwGetTime();

    while (!glfwWindowShouldClose(Window))
    {
        glfwPollEvents();

        double CurrentTime = glfwGetTime();
        float DeltaSeconds = (float)(CurrentTime - PreviousTime);
        PreviousTime = CurrentTime;
        if (DeltaSeconds <= 0.0f) 
        {
            DeltaSeconds = 1.0f / 60.0f;
        }

        BeginTextureMode(SceneTexture);
        ClearBackground(BLACK);

        BeginMode2D(Camera);
        DrawCircle((int)(ViewportTexW * 0.5f), (int)(ViewportTexH * 0.5f), 80.0f, RED);
        DrawText("raylib offscreen scene", 20, 20, 20, RAYWHITE);
        EndMode2D();
        EndTextureMode();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Controls");
            ImGui::Text("Delta: %.3f ms (%.1f FPS)", DeltaSeconds * 1000.0f, 1.0f / DeltaSeconds);
            ImGui::Text("Resize the window; raylib renders into a texture.");
            ImGui::End();
        }

        ImGui::Begin("Viewport");

        ImVec2 Avail = ImGui::GetContentRegionAvail();
        int DesiredW = (int)Avail.x;
        int DesiredH = (int)Avail.y;

        if (DesiredW > 0 && DesiredH > 0 && (DesiredW != ViewportTexW || DesiredH != ViewportTexH))
        {
            UnloadRenderTexture(SceneTexture);
            ViewportTexW = DesiredW;
            ViewportTexH = DesiredH;
            SceneTexture = CreateRenderTexture(ViewportTexW, ViewportTexH);

            Camera.offset = Vector2{ ViewportTexW * 0.5f, ViewportTexH * 0.5f };
        }

        ImTextureID TexID = (ImTextureID)(intptr_t)SceneTexture.texture.id;
        ImGui::Image(TexID, ImVec2((float)ViewportTexW, (float)ViewportTexH), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();

        ImGui::Render();

        int FrameBufferW, FrameBufferH;
        glfwGetFramebufferSize(Window, &FrameBufferW, &FrameBufferH);
        glViewport(0, 0, FrameBufferW, FrameBufferH);
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

        glfwSwapBuffers(Window);
    }

    UnloadRenderTexture(SceneTexture);

    rlglClose();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(Window);
    glfwTerminate();

    return 0;
}
