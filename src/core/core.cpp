//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <chrono>

#include "log_utils.hpp"
#include "key_standards.hpp"

#include "core/ee_core.hpp"
#include "graphics/ee_window.hpp"
#include "graphics/ee_scene.hpp"
#include "graphics/kw_window.hpp"
#include "core/kw_input.hpp"
#include "core/kg_context.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaKeyStandards::KeyboardButton;

using ElypsoEngine::Core::EngineCore;
using ElypsoEngine::Core::AppConfig;
using ElypsoEngine::Graphics::EngineWindow;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Core::Input;
using KalaGraphics::Core::GraphicsContext;
using KalaGraphics::Core::VSyncState;

using std::to_string;
using std::string;
using std::chrono::time_point;
using std::chrono::steady_clock;
using std::chrono::seconds;
using std::chrono::duration;
using std::format;

Input* input{};
GraphicsContext* gctx{};

static string GetFPS(f64 secondsToWait)
{
    static f64 cachedFPS = EngineCore::GetCurrentFPS();
    static time_point start = steady_clock::now();

    if (steady_clock::now() - start > duration<f64>(secondsToWait))
    {
        cachedFPS = EngineCore::GetCurrentFPS();
        start = steady_clock::now();
    }
    
    return format("{:.2f}", cachedFPS);
}

extern const AppConfig ElypsoEngine::Core::appConfig = 
{
    .title = "Metal Metropolis Pre-Release 1",
    .pos = 600
};

void ElypsoEngine::Core::Init()
{
    EngineWindow* ew = EngineWindow::GetRegistry().runtimeContent[0];
    ProcessWindow* pw = ProcessWindow::GetRegistry().GetContent(ew->GetWindowContextID());
    gctx = GraphicsContext::GetRegistry().GetContent(ew->GetGraphicsContextID());
    input = Input::GetRegistry().GetContent(pw->GetInputID());
}

void ElypsoEngine::Core::FixedUpdate()
{
    /*
    Log::Print(
        "This is a fixed update...",
        "MM_MAIN",
        LogType::LOG_DEBUG);
    */
}

void ElypsoEngine::Core::Update()
{
    Log::Print("fps: " + GetFPS(0.5));

    if (input->IsKeyPressed(KeyboardButton::K_1))
    {
        Log::Print("@@@@@ set vsync state to off");
        gctx->SetVSyncState(VSyncState::VSYNC_OFF);
    }
    if (input->IsKeyPressed(KeyboardButton::K_2))
    {
        Log::Print("@@@@@ set vsync state to adaptive");
        gctx->SetVSyncState(VSyncState::VSYNC_ON_ADAPTIVE);
    }
    if (input->IsKeyPressed(KeyboardButton::K_3))
    {
        Log::Print("@@@@@ set vsync state to triple buffered");
        gctx->SetVSyncState(VSyncState::VSYNC_ON_TRIPLE_BUFFERED);
    }
}