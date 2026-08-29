//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <array>
#include <vector>
#include <filesystem>
#include <string>

#include "core/kw_input.hpp"
#include "graphics/kw_window.hpp"
#include "core/kg_context.hpp"
#include "core/kg_viewport.hpp"
#include "resources/kg_shader.hpp"
#include "resources/kg_mesh.hpp"
#include "resources/kg_texture.hpp"
#include "resources/kg_camera.hpp"

namespace MetalMetropolis::Test
{
    using KalaWindow::Core::Input;
    using KalaWindow::Graphics::ProcessWindow;
    using KalaGraphics::Core::GraphicsContext;
    using KalaGraphics::Core::Viewport;
    using KalaGraphics::Resources::Shader;
    using KalaGraphics::Resources::Mesh;
    using KalaGraphics::Resources::Transform;
    using KalaGraphics::Resources::Vertex;
    using KalaGraphics::Resources::Vertex2D;
    using KalaGraphics::Resources::Texture;
    using KalaGraphics::Resources::TextureData;
    using KalaGraphics::Resources::Camera;

    using std::array;
    using std::vector;
    using std::filesystem::path;
    using std::string;

    class Examples
    {
    public:
        static string Test_Get_FPS(f64 secondsToWait);

        static void Test_Popup_And_File_Drag(ProcessWindow* pw);

        static void Test_VSync_Input(
            GraphicsContext* gctx,
            Input* input);

        static void Test_System_Sound_Input(Input* input);

        static void Test_Create_Notification(Input* input);

        static void Test_Get_Files(
            Input *input,
            vector<string>&& customTypes = {},
            path&& requiredRoot = {});

        static void Test_Window_Toggles(
            ProcessWindow* pw,
            Input* input);

        static void Test_Texture_Filter_Mode(
            Input* input,
            Texture* tex);

        static void Test_Camera_Toggle(Input* input);

        static void Test_Camera_Move(
            Input* input,
            Camera* cam,
            f32 deltaTime);

        static Shader* Test_Create_Shader(
            Viewport* viewport,
            bool is2D,
            array<path, 2>&& shaderFiles);

        static Texture* Test_Create_Texture(
            Shader* shader,
            TextureData&& textureData = {});

        static Mesh* Test_Create_Mesh(
            Shader* shader,
            Texture* texture,
            Transform&& transform = {},
            vector<Vertex>&& vertices = {},
            vector<u32>&& indices = {}); 

        static Mesh* Test_Create_Mesh(
            Shader* shader,
            Texture* texture,
            Transform&& transform = {},
            vector<Vertex2D>&& vertices = {},
            vector<u32>&& indices = {}); 

        static Camera* Test_Create_Camera(
            Shader* shader,
            Transform&& transform = {});
    };
}