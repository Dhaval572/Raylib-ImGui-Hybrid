#pragma once

#include "raylib.h"
#include <utility>

namespace Core {

    class ScopedTexture
    {
    public:
        ScopedTexture() = default;
        ScopedTexture(const char* path)
        {
            m_Texture = LoadTexture(path);
        }

        ~ScopedTexture()
        {
            if (m_Texture.id != 0)
                UnloadTexture(m_Texture);
        }

        // Move Only
        ScopedTexture(const ScopedTexture&) = delete;
        ScopedTexture& operator=(const ScopedTexture&) = delete;

        ScopedTexture(ScopedTexture&& other) noexcept
        {
            m_Texture = other.m_Texture;
            other.m_Texture = { 0 };
        }

        ScopedTexture& operator=(ScopedTexture&& other) noexcept
        {
            if (this != &other)
            {
                if (m_Texture.id != 0) UnloadTexture(m_Texture);
                m_Texture = other.m_Texture;
                other.m_Texture = { 0 };
            }
            return *this;
        }

        operator Texture2D() const { return m_Texture; }

    private:
        Texture2D m_Texture = { 0 };
    };

    class ScopedRenderTexture
    {
    public:
        ScopedRenderTexture() = default;
        ScopedRenderTexture(int width, int height)
        {
            m_Texture = LoadRenderTexture(width, height);
        }

        ~ScopedRenderTexture()
        {
            if (m_Texture.id != 0)
                UnloadRenderTexture(m_Texture);
        }

        void Load(int width, int height)
        {
            if (m_Texture.id != 0) UnloadRenderTexture(m_Texture);
            m_Texture = LoadRenderTexture(width, height);
        }

        // Move Only
        ScopedRenderTexture(const ScopedRenderTexture&) = delete;
        ScopedRenderTexture& operator=(const ScopedRenderTexture&) = delete;

        ScopedRenderTexture(ScopedRenderTexture&& other) noexcept
        {
            m_Texture = other.m_Texture;
            other.m_Texture = { 0 };
        }

        ScopedRenderTexture& operator=(ScopedRenderTexture&& other) noexcept
        {
            if (this != &other)
            {
                if (m_Texture.id != 0) UnloadRenderTexture(m_Texture);
                m_Texture = other.m_Texture;
                other.m_Texture = { 0 };
            }
            return *this;
        }

        operator RenderTexture2D() const { return m_Texture; }

        // Access members like a pointer if needed, or helper
        Texture2D GetTexture() const { return m_Texture.texture; }

    private:
        RenderTexture2D m_Texture = { 0 };
    };

}
