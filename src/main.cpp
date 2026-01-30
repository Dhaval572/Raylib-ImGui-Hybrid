#include "Core/Application/Application.h"


// The user application logic
class FSandboxApp : public Core::FApplication 
{
public:
    FSandboxApp() 
        : Core::FApplication(Core::FApplicationConfig{ .Name = "Raylib + ImGui Hybrid Engine", .Width = 1600, .Height = 900 })
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
    }
};

Core::FApplication* CreateApplication() 
{
    return new FSandboxApp();
}

#include "Core/Application/EntryPoint.h"