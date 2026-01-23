#include "Core/EntryPoint.h"
#include "imterm/terminal.hpp"
#include "imterm/terminal_helpers.hpp"

#include <cstdarg>
#include <cstdio>

// Global terminal pointer for the log callback
static ImTerm::terminal<ImTerm::terminal_helper_example<int>>* GlobalTerminal = nullptr;

// Custom Log Callback for Raylib
void CustomLogCallback(int msgType, const char* text, va_list args)
{
    if (!GlobalTerminal) return;

    // Filter out trace/debug logs if needed, but for now we capture all valid levels
    ImTerm::message::severity::severity_t Severity = ImTerm::message::severity::info;

    switch (msgType)
    {
    case LOG_TRACE: Severity = ImTerm::message::severity::trace; break;
    case LOG_DEBUG: Severity = ImTerm::message::severity::debug; break;
    case LOG_INFO:  Severity = ImTerm::message::severity::info; break;
    case LOG_WARNING: Severity = ImTerm::message::severity::warn; break;
    case LOG_ERROR: Severity = ImTerm::message::severity::err; break;
    case LOG_FATAL: Severity = ImTerm::message::severity::critical; break;
    default: break;
    }

    char Buffer[1024];
    vsnprintf(Buffer, sizeof(Buffer), text, args);

    ImTerm::message Msg;
    Msg.severity = Severity;
    Msg.value = std::string(Buffer);
    Msg.color_beg = Msg.color_end = 0; // No custom color range for now

    GlobalTerminal->add_message(std::move(Msg));
}



// The user application logic
class FSandboxApp : public Core::FApplication 
{
public:
    // Terminal
    using TerminalHelper = ImTerm::terminal_helper_example<int>; // Simple int state for now
    ImTerm::terminal<TerminalHelper> Terminal;
    int TerminalState = 0;

    FSandboxApp() 
        : Core::FApplication("Raylib + ImGui Hybrid Engine", 1600, 900)
        , Terminal(TerminalState, "Console") // Initialize Terminal with reference to state
    {
    }

    // Scene Resources
    RenderTexture2D SceneTexture = { 0 };
    int ViewportWidth = 0;
    int ViewportHeight = 0;
    Model CubeModel = { 0 };
    
    // Scene State
    Camera3D Camera = { 0 };
    float CubeRotation = 0.0f;
    float RotationSpeed = 1.0f;
    bool bDrawWireframe = false;
    bool bAutoRotate = true;

    // Visual Settings
    Color BgColor = { 25, 25, 25, 255 };
    Color CubeColor = { 230, 41, 55, 255 }; 
    Color GridColor = { 60, 60, 60, 255 };



    void OnStart() override 
    {
        // Terminal setup
        GlobalTerminal = &Terminal;
        SetTraceLogCallback(CustomLogCallback);

        Terminal.theme() = ImTerm::themes::cherry; // Matches our red theme nicely
        Terminal.set_min_log_level(ImTerm::message::severity::trace); // Capture everything
        Terminal.log_level_text() = std::nullopt; // Disable log level selector dropdown
        Terminal.set_autocomplete_pos(ImTerm::position::nowhere); // Disable autocomplete popup

        Terminal.add_text("Welcome to Raylib + ImGui Hybrid Engine!");
        Terminal.add_text("Type 'help' for commands (or 'echo <text>').");

        // Initialize Camera
        Camera.position = Vector3{ 4.0f, 4.0f, 4.0f };
        Camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
        Camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
        Camera.fovy = 45.0f;
        Camera.projection = CAMERA_PERSPECTIVE;

        // Initialize Render Texture (start small, will resize in Update)
        ViewportWidth = 1280;
        ViewportHeight = 720;
        SceneTexture = LoadRenderTexture(ViewportWidth, ViewportHeight);

        // Load a Unit Cube Model
        Mesh CubeMesh = GenMeshCube(1.5f, 1.5f, 1.5f);
        CubeModel = LoadModelFromMesh(CubeMesh);
    }

    void OnUpdate(float DeltaTime) override 
    {
        // Update Logic
        if (bAutoRotate) 
        {
            CubeRotation += (45.0f * DeltaTime * RotationSpeed);
            if (CubeRotation > 360.0f) CubeRotation -= 360.0f;
        }

        // --- Render Scene to Texture ---
        BeginTextureMode(SceneTexture);
        ClearBackground(BgColor);

        BeginMode3D(Camera);

            // Draw Grid
            DrawGrid(10, 1.0f);
            
            // Draw Axes
            DrawLine3D({0,0,0}, {1,0,0}, RED);
            DrawLine3D({0,0,0}, {0,1,0}, GREEN);
            DrawLine3D({0,0,0}, {0,0,1}, BLUE);

            // Draw Rotating Cube
            Vector3 CubePos = { 0.0f, 0.5f, 0.0f };
            Vector3 RotationAxis = { 0.0f, 1.0f, 0.0f };
            Vector3 Scale = { 1.0f, 1.0f, 1.0f };

            if (bDrawWireframe) 
            {
                DrawModelWiresEx
                (
                    CubeModel, 
                    CubePos, 
                    RotationAxis, 
                    CubeRotation, 
                    Scale, 
                    CubeColor
                );
            } 
            else 
            {
                DrawModelEx(CubeModel, CubePos, RotationAxis, CubeRotation, Scale, CubeColor);
                DrawModelWiresEx(CubeModel, CubePos, RotationAxis, CubeRotation, Scale, BLACK);
            }

        EndMode3D();
        EndTextureMode();
    }

    void OnUIRender() override 
    {
        // --- DockSpace ---
        ImGuiID DockSpaceId = ImGui::GetID("MyDockSpace");
        ImGui::DockSpaceOverViewport
        (
            DockSpaceId, 
            ImGui::GetMainViewport(), 
            ImGuiDockNodeFlags_PassthruCentralNode
        );

        // --- Settings Panel ---
        ImGui::Begin("Settings");
        
        ImGui::TextDisabled("Performance");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Separator();

        ImGui::TextDisabled("Scene Control");
        ImGui::Checkbox("Auto Rotate", &bAutoRotate);
        if (bAutoRotate) 
        {
             ImGui::SliderFloat("Speed", &RotationSpeed, 0.0f, 5.0f);
        } else 
        {
             ImGui::SliderFloat("Rotation", &CubeRotation, 0.0f, 360.0f);
        }
        ImGui::Checkbox("Wireframe Mode", &bDrawWireframe);
        
        ImGui::Separator();
        ImGui::TextDisabled("Colors");
        
        // Helper to convert Raylib Color to ImVec4 and back
        auto EditColor = [](const char* Label, Color& C) 
        {
            float Col[4] = { C.r / 255.0f, C.g / 255.0f, C.b / 255.0f, C.a / 255.0f };
            if (ImGui::ColorEdit4(Label, Col)) 
            {
                C.r = static_cast<unsigned char>(Col[0] * 255);
                C.g = static_cast<unsigned char>(Col[1] * 255);
                C.b = static_cast<unsigned char>(Col[2] * 255);
                C.a = static_cast<unsigned char>(Col[3] * 255);
            }
        };

        EditColor("Cube Color", CubeColor);
        EditColor("Background", BgColor);

        ImGui::End();

        // --- Terminal Window ---
        // We defer to ImTerm to handle its own window via show()
        // But we want it to be part of our docking layout
        if (ImGui::Begin("Console", nullptr)) 
        {
            Terminal.show();
        }
        ImGui::End();

        // --- Viewport Window ---
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport");
        
        ImVec2 ViewportPanelSize = ImGui::GetContentRegionAvail();
        int DesiredW = static_cast<int>(ViewportPanelSize.x);
        int DesiredH = static_cast<int>(ViewportPanelSize.y);

        // Resize texture if viewport changed size (and is valid)
        if 
        (
            DesiredW > 0 && 
            DesiredH > 0 && 
            (DesiredW != ViewportWidth || DesiredH != ViewportHeight)
        )
        {
            UnloadRenderTexture(SceneTexture);
            ViewportWidth = DesiredW;
            ViewportHeight = DesiredH;
            SceneTexture = LoadRenderTexture(ViewportWidth, ViewportHeight);
        }

        // Draw the texture
        // We flip the UVs (0,1) to (1,0) because Raylib renders upside down relative to ImGui/OpenGL coordinates
        ImTextureID TexID = (ImTextureID)(intptr_t)SceneTexture.texture.id;
        ImGui::Image
        (
            TexID, 
            ImVec2
            (
                static_cast<float>(ViewportWidth), 
                static_cast<float>(ViewportHeight)
            ), 
            ImVec2(0, 1), 
            ImVec2(1, 0)
        );

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void OnShutdown() override 
    {
        UnloadRenderTexture(SceneTexture);
        UnloadModel(CubeModel);
        
        SetTraceLogCallback(NULL);
        GlobalTerminal = nullptr;
    }
};

Core::FApplication* CreateApplication() 
{
    return new FSandboxApp();
}

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER

#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    return main(__argc, __argv);
}
#endif
