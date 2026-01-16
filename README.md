# Raylib + ImGui Hybrid

This project is a clean, modern template that combines **Raylib** with **Dear ImGui**.

It solves a common challenge: **How do I render my Raylib game inside an ImGui window?**

By rendering Raylib's output to an offscreen texture and passing it to ImGui, this template gives you a full "Game Engine Editor" layout with docking, resizing, and multiple viewports.

## 🚀 Why this template?

*   **Best of Both Worlds**: Use Raylib's friendly code for your game, and ImGui for your tools.
*   **Editor-Ready**: Your game runs inside a dockable, resizable panel—just like Unity or Unreal.
*   **Clean Architecture**: GLFW handles the windowing, preventing conflicts between Raylib and ImGui.
*   **Zero "DLL Hell"**: Raylib and ImGui are statically linked, so your build is self-contained.

## 🛠 Prerequisites

You'll just need standard C++ build tools:

*   **CMake** (3.21+)
*   **C++ Compiler** (Visual Studio 2022, GCC, or Clang)
*   **Git**

## 📦 Getting Started

### Windows (Visual Studio)

1.  **Configure**:
    ```powershell
    cmake -S . -B out/build
    ```
2.  **Build**:
    ```powershell
    cmake --build out/build --config Debug
    ```
3.  **Run**:
    ```powershell
    .\out\build\Debug\raylib_imgui_hybrid.exe
    ```

### Linux / macOS

1.  **Build & Run**:
    ```bash
    cmake -S . -B build
    cmake --build build
    ./build/raylib_imgui_hybrid
    ```

## 🧠 Under the Hood

Here is the magic behind the code:

1.  **Manual Start**: We wake up GLFW manually instead of using `InitWindow()`.
2.  **Raylib "Headless"**: We call `rlglInit()`, which tells Raylib "Get ready to draw, but don't open a window."
3.  **The Loop**:
    *   **Draw**: Render the game scene into a Raylib `RenderTexture2D`.
    *   **Display**: Pass that texture's ID to `ImGui::Image()`.
    *   **Resize**: If you resize the UI panel, we automatically resize the internal texture to match.

## 📂 Structure

*   `src/main.cpp`: The heart of the application.
*   `external/`: Where Raylib and ImGui live.
*   `CMakeLists.txt`: The build configuration.

---
*License: MIT*
