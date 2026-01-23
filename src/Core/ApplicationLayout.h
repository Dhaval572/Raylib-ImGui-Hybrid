#include<cstring>
#include<imgui.h>

// Default docking layout for the editor, loaded on startup
static constexpr std::string_view APPLICATION_DEFAULT_INI = 
R"(
[Window][Debug##Default]
Pos=1193,260
Size=400,400
Collapsed=0

[Window][Controls]
Pos=60,60
Size=360,65
Collapsed=0

[Window][Viewport]
Pos=0,0
Size=1600,900
Collapsed=0
DockId=0xE098E157,0

[Window][WindowOverViewport_11111111]
Pos=0,0
Size=1600,900
Collapsed=0

[Window][Settings]
Pos=36,36
Size=435,480
Collapsed=0

[Docking][Data]
DockSpace ID=0xE098E157 Window=0x1BBC0F80 Pos=0,0 Size=1600,900 CentralNode=1 HiddenTabBar=1 Selected=0xC450F867
)";

inline void LoadApplicationDefaultIni()
{
    ImGui::LoadIniSettingsFromMemory
    (
        APPLICATION_DEFAULT_INI.data(), 
        APPLICATION_DEFAULT_INI.size()
    );
}