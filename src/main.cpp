#include "Core/Application/Application.h"
#include "Core/Application/EntryPoint.h" // Includes main() and CreateApplication prototype
#include "Layers/ModelViewerLayer.h"

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

Core::FApplication* CreateApplication()
{
    return new FViewerApp();
}