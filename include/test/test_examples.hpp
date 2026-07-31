//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <array>
#include <vector>
#include <filesystem>
#include <string>

#include "core/kw_input.hpp"
#include "core/kg_context.hpp"
#include "resources/kg_mesh.hpp"

namespace MetalMetropolis::Test
{
    using KalaWindow::Core::Input;
    using KalaGraphics::Core::GraphicsContext;
    using KalaGraphics::Resources::Transform;
    using KalaGraphics::Resources::Vertex;

    using std::array;
    using std::vector;
    using std::filesystem::path;
    using std::string;

    class Examples
    {
    public:
        static string GetFPS(f64 secondsToWait);

        static void Test_VSync_Input(
            GraphicsContext* gctx,
            Input* input);

        static void Test_System_Sound_Input(Input* input);

        static void Create_Triangle(
            GraphicsContext* gctx,
            Transform&& triangleTransform,
            vector<Vertex>&& triangleVertices,
            array<path, 2>&& triangleShaders);  
    };
}