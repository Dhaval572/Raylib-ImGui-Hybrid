#pragma once

#include "raylib.h"
#include <utility>

namespace Core {

    class FScopedTexture
    {
    public:
        FScopedTexture() = default;
        FScopedTexture(const char* Path)
        {
            Texture = LoadTexture(Path);
        }

        ~FScopedTexture()
        {
            if (Texture.id != 0)
                UnloadTexture(Texture);
        }

        // Move Only
        FScopedTexture(const FScopedTexture&) = delete;
        FScopedTexture& operator=(const FScopedTexture&) = delete;

        FScopedTexture(FScopedTexture&& Other) noexcept
        {
            Texture = Other.Texture;
            Other.Texture = { 0 };
        }

        FScopedTexture& operator=(FScopedTexture&& Other) noexcept
        {
            if (this != &Other)
            {
                if (Texture.id != 0) UnloadTexture(Texture);
                Texture = Other.Texture;
                Other.Texture = { 0 };
            }
            return *this;
        }

        operator Texture2D() const { return Texture; }
        [[nodiscard]] bool IsValid() const { return Texture.id != 0; }

    private:
        Texture2D Texture = { 0 };
    };

    class FScopedRenderTexture
    {
    public:
        FScopedRenderTexture() = default;
        FScopedRenderTexture(int Width, int Height)
        {
            Texture = LoadRenderTexture(Width, Height);
        }

        ~FScopedRenderTexture()
        {
            if (Texture.id != 0)
                UnloadRenderTexture(Texture);
        }

        void Load(int Width, int Height)
        {
            if (Texture.id != 0) UnloadRenderTexture(Texture);
            Texture = LoadRenderTexture(Width, Height);
        }

        // Move Only
        FScopedRenderTexture(const FScopedRenderTexture&) = delete;
        FScopedRenderTexture& operator=(const FScopedRenderTexture&) = delete;

        FScopedRenderTexture(FScopedRenderTexture&& Other) noexcept
        {
            Texture = Other.Texture;
            Other.Texture = { 0 };
        }

        FScopedRenderTexture& operator=(FScopedRenderTexture&& Other) noexcept
        {
            if (this != &Other)
            {
                if (Texture.id != 0) UnloadRenderTexture(Texture);
                Texture = Other.Texture;
                Other.Texture = { 0 };
            }
            return *this;
        }

        operator RenderTexture2D() const { return Texture; }

        // Access members like a pointer if needed, or helper
        [[nodiscard]] Texture2D GetTexture() const { return Texture.texture; }
        [[nodiscard]] bool IsValid() const { return Texture.id != 0; }

    private:
        RenderTexture2D Texture = { 0 };
    };

    class FScopedModel
    {
    public:
        FScopedModel() = default;
        explicit FScopedModel(const Mesh& InMesh)
        {
            ModelResource = LoadModelFromMesh(InMesh);
        }

        ~FScopedModel()
        {
            // Check if model has valid meshes before unloading to avoid raylib warnings/errors if empty
            if (ModelResource.meshCount > 0)
                UnloadModel(ModelResource);
        }

        void Set(const Model& InModel)
        {
             if (ModelResource.meshCount > 0) UnloadModel(ModelResource);
             ModelResource = InModel;
        }

        // Move Only
        FScopedModel(const FScopedModel&) = delete;
        FScopedModel& operator=(const FScopedModel&) = delete;

        FScopedModel(FScopedModel&& Other) noexcept
        {
            ModelResource = Other.ModelResource;
            Other.ModelResource = { 0 };
        }

        FScopedModel& operator=(FScopedModel&& Other) noexcept
        {
             if (this != &Other)
             {
                 if (ModelResource.meshCount > 0) UnloadModel(ModelResource);
                 ModelResource = Other.ModelResource;
                 Other.ModelResource = { 0 };
             }
             return *this;
        }

        operator Model() const { return ModelResource; }
        [[nodiscard]] bool IsValid() const { return ModelResource.meshCount > 0; }

    private:
        Model ModelResource = { 0 };
    };

}
