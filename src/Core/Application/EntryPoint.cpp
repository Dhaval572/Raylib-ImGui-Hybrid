#include "Application.h"
#include <memory>
    
extern Core::FApplication* CreateApplication();

int main([[maybe_unused]]int argc, [[maybe_unused]] char** argv)
{
    auto App = std::unique_ptr<Core::FApplication>(CreateApplication());
    if(App) App->Run();
    return 0;
}
