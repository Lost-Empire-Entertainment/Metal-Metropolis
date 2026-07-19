//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "log_utils.hpp"
#include "key_standards.hpp"

#include "core/ee_core.hpp"
#include "graphics/ee_window.hpp"
#include "graphics/ee_scene.hpp"
#include "graphics/kw_window.hpp"
#include "core/kw_input.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaKeyStandards::KeyboardButton;
using KalaHeaders::KalaKeyStandards::MouseButton;

using ElypsoEngine::Core::AppConfig;
using ElypsoEngine::Graphics::EngineWindow;
using ElypsoEngine::Graphics::Scene;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Core::Input;
using KalaWindow::Core::InputCode;

using std::to_string;

static constexpr InputCode ic[] = 
{
    { .kb = KeyboardButton::K_LEFT_CTRL },
    { .mb = MouseButton::M_MIDDLE }
};

Input* input{};

extern const AppConfig ElypsoEngine::Core::appConfig = 
{
    .title = "Metal Metropolis Pre-Release 1",
    .pos = 600
};

void ElypsoEngine::Core::Init()
{
    EngineWindow* ew = EngineWindow::GetRegistry().runtimeContent[0];
    ProcessWindow* pw = ProcessWindow::GetRegistry().GetContent(ew->GetWindowContextID());
    input = Input::GetRegistry().GetContent(pw->GetInputID());

    pw->SetMinSize({ 800, 600 });

    Scene* activeScene = Scene::GetRegistry().GetContent(ew->GetActiveSceneID());

    if (!activeScene)
    {
        Log::Print(
            "Failed to get active scene title because scene '" + to_string(ew->GetActiveSceneID()) + "' was not found!",
            "MM_MAIN",
            LogType::LOG_ERROR,
            2);

        return;
    }

    Log::Print(
        "Scene name: " + string(activeScene->GetTitle()),
        "MM_MAIN",
        LogType::LOG_INFO);
}

void ElypsoEngine::Core::Update()
{
    if (input->IsComboPressed(ic))
    {
        Log::Print("pressed left ctrl + middle mouse button, calling engine shutdown from game");
        EngineCore::Shutdown();
    }
}