// File: MeshEditor/src/engine/Time.cpp
#include "Time.h"

#include <chrono>

namespace mesh
{
namespace
{
    using Clock = std::chrono::steady_clock;
    Clock::time_point g_last;
    float g_delta = 0.0f;
    float g_total = 0.0f;
}

void Time::Tick()
{
    auto now = Clock::now();
    if (g_last.time_since_epoch().count() == 0)
    {
        g_last = now;
        g_delta = 0.0f;
        return;
    }
    std::chrono::duration<float> diff = now - g_last;
    g_last = now;
    g_delta = diff.count();
    g_total += g_delta;
}

float Time::DeltaSeconds()
{
    return g_delta;
}

float Time::TotalSeconds()
{
    return g_total;
}
}
