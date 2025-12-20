// File: MeshEditor/src/engine/Time.cpp
#include "Time.h"

namespace MeshEditor
{
std::chrono::steady_clock::time_point Time::s_last = std::chrono::steady_clock::now();
float Time::s_delta = 0.0f;

float Time::DeltaTime()
{
    return s_delta;
}

void Time::Tick()
{
    auto now = std::chrono::steady_clock::now();
    s_delta = std::chrono::duration<float>(now - s_last).count();
    s_last = now;
}
}
