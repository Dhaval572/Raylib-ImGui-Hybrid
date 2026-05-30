#include "Core/Application/Application.h"
#include "Core/Application/EntryPoint.h" // Includes main() and CreateApplication prototype
#include "Layers/ModelViewerLayer.h"
#include <memory>

class FViewerApp : public Core::FApplication
{
public:
    FViewerApp()
        : Core::FApplication
          (
              Core::FApplicationConfig
              {
                  .Name = "Raylib 3D Viewer",
                  .Width = 1600,
                  .Height = 900
              }
          )
    {
        PushLayer(new Core::ModelViewerLayer());
    }
};

std::unique_ptr<Core::FApplication> CreateApplication()
{
    return std::make_unique<FViewerApp>();
}
