#pragma once
#include "Application.h"

extern Core::FApplication* CreateApplication();

int main(int argc, char** argv)
{
    Core::FApplication* App = CreateApplication();
    App->Run();
    delete App;
    return 0;
}
