#include "ApplicationTheme.h"
#include <imgui.h>

namespace Core 
{
    void SetApplicationTheme(std::string_view Path)
    {
        ImGuiIO& IO = ImGui::GetIO();
        IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // Fonts
        IO.Fonts->Clear();
        ImFontConfig FontConfig;
        FontConfig.OversampleH = 2;
        FontConfig.OversampleV = 2;
        FontConfig.PixelSnapH = true;

        IO.Fonts->AddFontFromFileTTF(Path.data(), 18.0f, &FontConfig);

        ImGuiStyle& Style = ImGui::GetStyle();
        
        // Modern Dark Theme Palette
        ImVec4 BgDark       = ImVec4(0.10f, 0.105f, 0.11f, 1.0f); // Base Dark
        ImVec4 BgLight      = ImVec4(0.15f, 0.16f, 0.17f, 1.0f);  // Lighter Panel
        ImVec4 BgMedium     = ImVec4(0.12f, 0.13f, 0.14f, 1.0f);  // In-between
        
        ImVec4 Accent       = ImVec4(0.20f, 0.45f, 0.90f, 1.0f);  // Modern Blue Accent
        ImVec4 AccentHover  = ImVec4(0.30f, 0.55f, 0.95f, 1.0f);
        ImVec4 AccentActive = ImVec4(0.15f, 0.40f, 0.85f, 1.0f);
        
        ImVec4 Text         = ImVec4(0.90f, 0.90f, 0.92f, 1.0f);
        ImVec4 TextDisabled = ImVec4(0.50f, 0.50f, 0.55f, 1.0f);
        ImVec4 Border       = ImVec4(0.25f, 0.25f, 0.27f, 0.5f);

        // Colors
        Style.Colors[ImGuiCol_WindowBg]         = BgDark;
        Style.Colors[ImGuiCol_ChildBg]          = BgMedium;
        Style.Colors[ImGuiCol_PopupBg]          = BgLight;
        Style.Colors[ImGuiCol_Border]           = Border;
        Style.Colors[ImGuiCol_BorderShadow]     = ImVec4(0, 0, 0, 0);
        
        Style.Colors[ImGuiCol_FrameBg]          = BgMedium;
        Style.Colors[ImGuiCol_FrameBgHovered]   = BgLight;
        Style.Colors[ImGuiCol_FrameBgActive]    = BgLight;
        
        Style.Colors[ImGuiCol_TitleBg]          = BgDark;
        Style.Colors[ImGuiCol_TitleBgActive]    = BgDark; // Flat title bar
        Style.Colors[ImGuiCol_TitleBgCollapsed] = BgDark;
        
        Style.Colors[ImGuiCol_MenuBarBg]        = BgDark;
        
        // Scrollbar
        Style.Colors[ImGuiCol_ScrollbarBg]      = BgDark;
        Style.Colors[ImGuiCol_ScrollbarGrab]    = BgLight;
        Style.Colors[ImGuiCol_ScrollbarGrabHovered] = Accent;
        Style.Colors[ImGuiCol_ScrollbarGrabActive]  = AccentActive;
        
        // Checkmark / Slider
        Style.Colors[ImGuiCol_CheckMark]        = Accent;
        Style.Colors[ImGuiCol_SliderGrab]       = Accent;
        Style.Colors[ImGuiCol_SliderGrabActive] = AccentActive;
        
        // Button
        Style.Colors[ImGuiCol_Button]           = BgLight;
        Style.Colors[ImGuiCol_ButtonHovered]    = AccentHover;
        Style.Colors[ImGuiCol_ButtonActive]     = AccentActive;
        
        // Header
        Style.Colors[ImGuiCol_Header]           = BgLight;
        Style.Colors[ImGuiCol_HeaderHovered]    = AccentHover;
        Style.Colors[ImGuiCol_HeaderActive]     = AccentActive;
        
        // Separator
        Style.Colors[ImGuiCol_Separator]        = Border;
        Style.Colors[ImGuiCol_SeparatorHovered] = AccentHover;
        Style.Colors[ImGuiCol_SeparatorActive]  = AccentActive;
        
        // Resize Grip
        Style.Colors[ImGuiCol_ResizeGrip]       = ImVec4(0, 0, 0, 0);
        Style.Colors[ImGuiCol_ResizeGripHovered]= Accent;
        Style.Colors[ImGuiCol_ResizeGripActive] = AccentActive;
        
        // Tabs
        Style.Colors[ImGuiCol_Tab]              = BgDark;
        Style.Colors[ImGuiCol_TabHovered]       = AccentHover;
        Style.Colors[ImGuiCol_TabActive]        = BgLight; // Active tab mimics window bg
        Style.Colors[ImGuiCol_TabUnfocused]     = BgDark;
        Style.Colors[ImGuiCol_TabUnfocusedActive] = BgLight;
        
        // Docking
        Style.Colors[ImGuiCol_DockingPreview]   = Accent;
        Style.Colors[ImGuiCol_DockingEmptyBg]   = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);
        
        // Text
        Style.Colors[ImGuiCol_Text]             = Text;
        Style.Colors[ImGuiCol_TextDisabled]     = TextDisabled;
        Style.Colors[ImGuiCol_TextSelectedBg]   = ImVec4(Accent.x, Accent.y, Accent.z, 0.25f);
        Style.Colors[ImGuiCol_NavHighlight]     = ImVec4(0, 0, 0, 0); // Hide nav highlight for cleaner look
        
        // Styling geometry
        Style.WindowPadding     = ImVec2(12, 12);
        Style.FramePadding      = ImVec2(8, 5);
        Style.CellPadding       = ImVec2(8, 4);
        Style.ItemSpacing       = ImVec2(8, 8);
        Style.ItemInnerSpacing  = ImVec2(6, 6);
        
        Style.ScrollbarSize     = 12.0f;
        Style.GrabMinSize       = 10.0f;
        
        Style.WindowBorderSize  = 1.0f;
        Style.ChildBorderSize   = 1.0f;
        Style.PopupBorderSize   = 1.0f;
        Style.FrameBorderSize   = 0.0f;
        
        Style.WindowRounding    = 4.0f;
        Style.ChildRounding     = 4.0f;
        Style.FrameRounding     = 4.0f;
        Style.PopupRounding     = 4.0f;
        Style.ScrollbarRounding = 9.0f;
        Style.GrabRounding      = 4.0f;
        Style.TabRounding       = 4.0f;
        
        // Alignment
        Style.WindowTitleAlign = ImVec2(0.02f, 0.5f); // Left aligned titles
    }
}
