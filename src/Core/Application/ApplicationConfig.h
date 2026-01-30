#pragma once
#include <string>

namespace Core {

    struct FApplicationConfig
    {
        std::string Name = "Raylib Hybrid App";
        int Width = 1280;
        int Height = 720;
        bool bVSync = true;
        bool bMaximized = false;
        
        // Resource paths
        std::string FontPath = "Roboto-Regular.ttf";
        float FontSize = 20.0f;
    };

}
