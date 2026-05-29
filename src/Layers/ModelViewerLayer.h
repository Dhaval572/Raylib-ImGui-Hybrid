#pragma once

#include "Core/Layers/Layer.h"
#include "Core/Renderer/ScopedResources.h"
#include "Core/Events/MouseEvent.h"
#include "Core/Events/ApplicationEvent.h"
#include "raylib.h"
#include <mutex>

namespace Core
{
    class ModelViewerLayer : public FLayer
    {
    public:
        ModelViewerLayer();
        virtual ~ModelViewerLayer() = default;

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float DeltaTime) override;
        void OnUIRender() override;
        void OnEvent(FEvent& Event) override;

    private:
        bool OnWindowDrop(class FWindowDropEvent& e);
        bool OnMouseScrolled(class FMouseScrolledEvent& e);
        bool OnMouseButtonPressed(class FMouseButtonPressedEvent& e);

    private:
        // Scene Resources
        FScopedRenderTexture ViewportTexture;
        FScopedModel LoadedModel;

        // Scene State
        Camera3D EditorCamera = {};
        Vector3 CameraTarget = { 0.0f, 0.0f, 0.0f };
        Vector2 LastMousePos = { 0.0f, 0.0f };
        float CameraDistance = 5.0f;
        float CameraAngleX = 0.0f;
        float CameraAngleY = 0.5f;

        // Viewport Settings
        int ViewportWidth = 1280;
        int ViewportHeight = 720;
        bool bViewportFocused = false;
        bool bViewportHovered = false;

        // Visual Settings
        Color BgColor = { 30, 30, 30, 255 };
        bool bDrawGrid = true;
        bool bDrawWireframe = false;
        bool bAutoRotate = false;
        float RotationSpeed = 0.5f;


        // Transform
        Vector3 ModelPosition = { 0.0f, 0.0f, 0.0f };
        Vector3 ModelRotation = { 0.0f, 0.0f, 0.0f };
        Vector3 ModelScale = { 1.0f, 1.0f, 1.0f };

        // Stats
        std::string CurrentFilePath = "Drag a 3D model here";

        // Threading
        std::mutex LoadMutex;
        std::string PendingPath;
    };
}
