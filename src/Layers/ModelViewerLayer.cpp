#include "ModelViewerLayer.h"
#include "Core/Renderer/ModelLoader.h"
#include "Core/Input/Input.h"
#include "Core/Logging/Log.h"
#include "Core/Events/MouseEvent.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <raymath.h>
#include <format>
#include <cmath>

namespace Core
{
    static void DrawVec3Control
    (
        const std::string& label,
        Vector3& values,
        float resetValue = 0.0f,
        float columnWidth = 100.0f
    )
    {
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text("%s", label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        // X Axis
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        if (ImGui::Button("X", buttonSize)) values.x = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // Y Axis
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        if (ImGui::Button("Y", buttonSize)) values.y = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // Z Axis
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        if (ImGui::Button("Z", buttonSize)) values.z = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);

        ImGui::PopID();
        ImGui::Spacing();
    }

    ModelViewerLayer::ModelViewerLayer()
        : FLayer("ModelViewer")
    {
    }

    void ModelViewerLayer::OnAttach()
    {
        // Init Camera
        EditorCamera.position = Vector3{ 5.0f, 5.0f, 5.0f };
        EditorCamera.target = Vector3{ 0.0f, 0.0f, 0.0f };
        EditorCamera.up = Vector3{ 0.0f, 1.0f, 0.0f };
        EditorCamera.fovy = 45.0f;
        EditorCamera.projection = CAMERA_PERSPECTIVE;

        // Init Texture
        ViewportTexture.Load(ViewportWidth, ViewportHeight);

        // Load Default Cube so it's not empty
        Mesh DefaultMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
        LoadedModel.Set(LoadModelFromMesh(DefaultMesh));
    }

    void ModelViewerLayer::OnDetach()
    {
        // RAII handles cleanup
    }

    void ModelViewerLayer::OnUpdate(float DeltaTime)
    {
        // 0. Deferred Loading (Handle pending drops from Main Thread)
        {
            std::string PathToLoad;
            {
                std::lock_guard<std::mutex> Lock(LoadMutex);
                if (!PendingPath.empty())
                {
                    PathToLoad = PendingPath;
                    PendingPath.clear();
                }
            }

            if (!PathToLoad.empty())
            {
                if
                (
                    IsFileExtension(PathToLoad.c_str(), ".obj")  ||
                    IsFileExtension(PathToLoad.c_str(), ".gltf") ||
                    IsFileExtension(PathToLoad.c_str(), ".glb")  ||
                    IsFileExtension(PathToLoad.c_str(), ".iqm")  ||
                    IsFileExtension(PathToLoad.c_str(), ".fbx")
                )
                {
                    Model NewModel = ModelLoader::LoadModel(PathToLoad); // Use our custom loader abstraction

                    // Validate model
                    if (NewModel.meshCount > 0)
                    {
                        LoadedModel.Set(NewModel);
                        CurrentFilePath = PathToLoad;
                        FLog::Debug("Loaded Model (Render Thread): {}", PathToLoad);

                        // Calculate Bounding Box to center and scale camera
                        BoundingBox Bounds = GetModelBoundingBox(LoadedModel);

                        Vector3 Center =
                        {
                            (Bounds.min.x + Bounds.max.x) / 2.0f,
                            (Bounds.min.y + Bounds.max.y) / 2.0f,
                            (Bounds.min.z + Bounds.max.z) / 2.0f
                        };

                        Vector3 Size =
                        {
                            fabsf(Bounds.max.x - Bounds.min.x),
                            fabsf(Bounds.max.y - Bounds.min.y),
                            fabsf(Bounds.max.z - Bounds.min.z)
                        };

                        float MaxDim = fmaxf(Size.x, fmaxf(Size.y, Size.z));
                        if (MaxDim <= 0.0f) MaxDim = 1.0f;

                        // Adjust Camera to fit
                        CameraTarget = Center;
                        CameraDistance = MaxDim * 2.5f; // Pull back enough to see it

                        // Reset rotation to look at it nicely
                        CameraAngleX = 0.0f;
                        CameraAngleY = 0.5f;

                        FLog::Debug
                        (
                            "Model Bounds: Min({:.1f}, {:.1f}, {:.1f}) Max({:.1f}, {:.1f}, {:.1f}) Size({:.1f})",
                            Bounds.min.x, Bounds.min.y, Bounds.min.z,
                            Bounds.max.x, Bounds.max.y, Bounds.max.z,
                            MaxDim
                        );
                    }
                    else
                    {
                        FLog::Error("Model loaded but has no meshes: {}", PathToLoad);
                        UnloadModel(NewModel);
                    }
                }
                else
                {
                    FLog::Error("Unsupported file extension: {}", PathToLoad);
                }
            }
        }

        // 1. Handle Input for Blender-Style Orbit Camera
        // MLB = Orbit, Shift+MLB = Pan, Scroll = Zoom

        bool bLeftClick = FInput::IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        bool bShiftDown = FInput::IsKeyPressed(KEY_LEFT_SHIFT) 
                        || FInput::IsKeyPressed(KEY_RIGHT_SHIFT);

        if (bViewportHovered || bViewportFocused)
        {
            // Calculate Mouse Delta manually since Raylib's PollInputEvents isn't called
            auto [Mx, My] = FInput::GetMousePosition();
            Vector2 CurrentMouse = { Mx, My };
            Vector2 MouseDelta = { CurrentMouse.x - LastMousePos.x, CurrentMouse.y - LastMousePos.y };

            // Only apply delta if we are dragging, otherwise it's just movement
            // But we need to track LastPos regardless

            float Sensitivity = 0.005f;

            if (bLeftClick)
            {
                if (bShiftDown)
                {
                    // Pan (Shift + MMB)
                    // Move target in camera's local right and up directions
                    float PanSpeed = 0.01f * CameraDistance;

                    // Calculate camera right and up vectors
                    Vector3 Forward =
                    {
                        CameraTarget.x - EditorCamera.position.x,
                        CameraTarget.y - EditorCamera.position.y,
                        CameraTarget.z - EditorCamera.position.z
                    };

                    // Normalize forward
                    float Len = sqrtf(Forward.x*Forward.x + Forward.y*Forward.y + Forward.z*Forward.z);
                    if (Len > 0.0f) { Forward.x /= Len; Forward.y /= Len; Forward.z /= Len; }

                    // Right = Forward cross Up
                    Vector3 Right =
                    {
                        Forward.z * EditorCamera.up.y - Forward.y * EditorCamera.up.z,
                        Forward.x * EditorCamera.up.z - Forward.z * EditorCamera.up.x,
                        Forward.y * EditorCamera.up.x - Forward.x * EditorCamera.up.y
                    };

                    // Normalize right
                    Len = sqrtf(Right.x*Right.x + Right.y*Right.y + Right.z*Right.z);
                    if (Len > 0.0f) { Right.x /= Len; Right.y /= Len; Right.z /= Len; }

                    // Local Up = Right cross Forward
                    Vector3 Up =
                    {
                        Right.y * Forward.z - Right.z * Forward.y,
                        Right.z * Forward.x - Right.x * Forward.z,
                        Right.x * Forward.y - Right.y * Forward.x
                    };

                    CameraTarget.x -= Right.x * MouseDelta.x * PanSpeed;
                    CameraTarget.y -= Right.y * MouseDelta.x * PanSpeed;
                    CameraTarget.z -= Right.z * MouseDelta.x * PanSpeed;

                    CameraTarget.x += Up.x * MouseDelta.y * PanSpeed;
                    CameraTarget.y += Up.y * MouseDelta.y * PanSpeed;
                    CameraTarget.z += Up.z * MouseDelta.y * PanSpeed;
                }
                else
                {
                    // Orbit (MMB only)
                    CameraAngleX -= MouseDelta.x * Sensitivity;
                    CameraAngleY -= MouseDelta.y * Sensitivity;
                    CameraAngleY = std::clamp(CameraAngleY, -1.55f, 1.55f);
                }
            }

            LastMousePos = CurrentMouse;
        }
        else
        {
            // Sync LastMousePos even when not hovering to prevent jumps on re-entry
             auto [Mx, My] = FInput::GetMousePosition();
             LastMousePos = { Mx, My };
        }

        // Calculate Camera Position
        // Spherical coordinates
        float X = CameraDistance * cosf(CameraAngleY) * sinf(CameraAngleX);
        float Y = CameraDistance * sinf(CameraAngleY);
        float Z = CameraDistance * cosf(CameraAngleY) * cosf(CameraAngleX);

        EditorCamera.position = Vector3{ X, Y, Z };
        EditorCamera.target = CameraTarget;

        // Auto Rotate Model
        if (bAutoRotate)
        {
            ModelRotation.y += (90.0f * DeltaTime * RotationSpeed);
            if (ModelRotation.y > 360.0f) ModelRotation.y -= 360.0f;
        }

        // 2. Render Scene
        if (ViewportTexture.IsValid())
        {
            BeginTextureMode(ViewportTexture);
            ClearBackground(BgColor);

            BeginMode3D(EditorCamera);

                if (bDrawGrid) DrawGrid(20, 1.0f);

                // Draw Axes
                DrawLine3D({0,0,0}, {1,0,0}, RED);
                DrawLine3D({0,0,0}, {0,1,0}, GREEN);
                DrawLine3D({0,0,0}, {0,0,1}, BLUE);

                if (LoadedModel.IsValid())
                {
                    Model& M = LoadedModel.Get();

                    // Apply Transform
                    // Reconstruct transform matrix (Scale -> Rotate -> Translate)
                    Matrix matScale = MatrixScale(ModelScale.x, ModelScale.y, ModelScale.z);
                    Matrix matRot = MatrixRotateXYZ
                    (
                        {
                            DEG2RAD * ModelRotation.x,
                            DEG2RAD * ModelRotation.y,
                            DEG2RAD * ModelRotation.z
                        }
                    );
                    Matrix matTrans = 
                        MatrixTranslate(ModelPosition.x, ModelPosition.y, ModelPosition.z);

                    M.transform = MatrixMultiply(MatrixMultiply(matScale, matRot), matTrans);

                    if (bDrawWireframe)
                    {
                        /* DrawModelWires uses the model's transform if we pass
                         * identity position/rotation/scale to the function? */
                        // DrawModelWires(model, position, scale, color);
                        // It applies position and scale ON TOP of model.transform.
                        // So we pass 0,0,0 and 1.0f.
                        DrawModelWires(M, {0,0,0}, 1.0f, WHITE);
                    }
                    else
                    {
                        DrawModel(M, {0,0,0}, 1.0f, WHITE);
                    }
                }

            EndMode3D();
            EndTextureMode();
        }
    }

    void ModelViewerLayer::OnUIRender()
    {
        // Dockspace
        ImGuiID DockIO = ImGui::GetID("ViewerDockspace");
        ImGui::DockSpaceOverViewport
        (
            DockIO,
            ImGui::GetMainViewport(),
            ImGuiDockNodeFlags_PassthruCentralNode
        );

        // Viewport Window
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport");

        bViewportHovered = ImGui::IsWindowHovered();
        bViewportFocused = ImGui::IsWindowFocused();

        ImVec2 PanelSize = ImGui::GetContentRegionAvail();
        int NewW = (int)PanelSize.x;
        int NewH = (int)PanelSize.y;

        if (NewW > 0 && NewH > 0 && (NewW != ViewportWidth || NewH != ViewportHeight))
        {
            ViewportWidth = NewW;
            ViewportHeight = NewH;
            ViewportTexture.Load(ViewportWidth, ViewportHeight);
        }

        if (ViewportTexture.IsValid())
        {
             ImTextureID TexID = (ImTextureID)(intptr_t)ViewportTexture.GetTexture().id;
             // Flip UV
             ImGui::Image(TexID, PanelSize, ImVec2(0, 1), ImVec2(1, 0));
        }

        // Drag Drop Overlay text
        if (!LoadedModel.IsValid())
        {
            ImVec2 Pos = ImGui::GetWindowPos();
            ImGui::GetWindowDrawList()->AddText
            (
                ImVec2(Pos.x + 20, Pos.y + 40),
                IM_COL32(255, 255, 255, 255),
                "Drag & Drop Model Here"
            );
        }

        ImGui::End();
        ImGui::PopStyleVar();

        // Inspector Window
        ImGui::Begin("Inspector");

        ImGui::TextDisabled("File Info");
        ImGui::TextWrapped("Path: %s", CurrentFilePath.c_str());

        ImGui::Separator();
        ImGui::TextDisabled("Mesh Stats");
        if (LoadedModel.IsValid())
        {
            Model& M = LoadedModel.Get(); // Cast to raw for access
            ImGui::Text("Mesh Count: %d", M.meshCount);
            ImGui::Text("Material Count: %d", M.materialCount);
            if (M.meshCount > 0)
            {
                ImGui::Text("Vertices: %d", M.meshes[0].vertexCount);
                ImGui::Text("Triangles: %d", M.meshes[0].triangleCount);
            }
        }
        else
        {
            ImGui::Text("No Model Loaded");
        }

        ImGui::Separator();
        ImGui::TextDisabled("Transform");
        DrawVec3Control("Location", ModelPosition);
        DrawVec3Control("Rotation", ModelRotation);
        DrawVec3Control("Scale", ModelScale, 1.0f);

        ImGui::Separator();
        ImGui::TextDisabled("Settings");
        ImGui::Checkbox("Wireframe", &bDrawWireframe);
        ImGui::Checkbox("Grid", &bDrawGrid);
        ImGui::Checkbox("Auto Rotate", &bAutoRotate);
        if (bAutoRotate) ImGui::SliderFloat("Speed", &RotationSpeed, 0.0f, 5.0f);

        float CamDist = CameraDistance;
        if (ImGui::SliderFloat("Zoom", &CamDist, 1.0f, 20.0f))
        {
            CameraDistance = CamDist;
        }

        ImGui::End();
    }

    void ModelViewerLayer::OnEvent(FEvent& Event)
    {
        FEventDispatcher Dispatcher(Event);
        Dispatcher.Dispatch<FWindowDropEvent>(CORE_BIND_EVENT_FN(OnWindowDrop));
        Dispatcher.Dispatch<FMouseScrolledEvent>(CORE_BIND_EVENT_FN(OnMouseScrolled));
    }

    bool ModelViewerLayer::OnWindowDrop(FWindowDropEvent& e)
    {
        const auto& Paths = e.GetPaths();
        if (Paths.empty()) return false;

        std::string Path = Paths[0];

        // Defer loading to Render Thread
        {
            std::lock_guard<std::mutex> Lock(LoadMutex);
            PendingPath = Path;
        }

        FLog::Debug("Queueing Model Load: {}", Path);
        return true;
    }

    bool ModelViewerLayer::OnMouseScrolled(FMouseScrolledEvent& e)
    {
        if (bViewportHovered)
        {
            CameraDistance -= e.GetYOffset() * 0.5f;
            if (CameraDistance < 1.0f) CameraDistance = 1.0f;
            return true;
        }
        return false;
    }

}
