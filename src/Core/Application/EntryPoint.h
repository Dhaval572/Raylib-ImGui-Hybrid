#pragma once
#include "Application.h"

#include <memory> 

extern Core::FApplication* CreateApplication();

int main(int argc, char** argv)
{
    auto App = std::unique_ptr<Core::FApplication>(CreateApplication());
    App->Run();
    return 0;
}


