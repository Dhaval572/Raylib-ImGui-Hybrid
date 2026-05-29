#pragma once

#include "raylib.h"
#include <string> // IWYU pragma: keep

namespace Core 
{

    class ModelLoader
    {
    public:
        // Returns an invalid model (meshCount == 0) on failure
        static Model LoadModel(std::string_view Path);
        
    private:
        static Model LoadFBX(std::string_view Path);
    };

}
