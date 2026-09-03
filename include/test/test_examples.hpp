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
#include "core/kg_hit_test.hpp"
#include "core/kg_shader.hpp"
#include "resources/kg_mesh.hpp"
#include "resources/kg_texture.hpp"
#include "resources/kg_camera.hpp"
#include "import/kg_import_font.hpp"

namespace MetalMetropolis::Test
{
    using KalaWindow::Core::Input;
    using KalaWindow::Graphics::ProcessWindow;
    using KalaGraphics::Core::GraphicsContext;
    using KalaGraphics::Core::Viewport;
    using KalaGraphics::Core::HitTest;
    using KalaGraphics::Core::Shader;
    using KalaGraphics::Resources::Mesh;
    using KalaGraphics::Resources::MeshData;
    using KalaGraphics::Resources::Texture;
    using KalaGraphics::Resources::TextureData;
    using KalaGraphics::Resources::Camera;
    using KalaGraphics::Import::ImportFont;

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

        static void Test_Toggle_From_Atlas_State(Input* input);
        static void Test_Print_Glyph_To_Console(
            Input* input,
            ImportFont* font);
        static void Test_Print_Glyph_To_Texture(
            Input* input,
            ImportFont* font,
            Texture* fontTexture,
            Mesh* fontMesh,
            Mesh* fontBackgroundMesh);
        static void Test_Print_Glyph_Atlas_To_Texture(
            ImportFont* font,
            Texture* fontTexture,
            Mesh* fontMesh,
            Mesh* fontBackgroundMesh);

        static void Test_Mesh_Toggle_Recreate_Target(Input* input);

        static void Test_Mesh_Recreate_Cube_On_Mouse_Actions(
            Input* input,
            Mesh* mesh);
        static void Test_Mesh_Recreate_Pyramid_On_Mouse_Actions(
            Input* input,
            Mesh* mesh);
        static void Test_Mesh_Recreate_Sphere_On_Mouse_Actions(
            Input* input,
            Mesh* mesh);

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
            MeshData&& meshData);

        static Camera* Test_Create_Camera(Shader* shader);
    };
}