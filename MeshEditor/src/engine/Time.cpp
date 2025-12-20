// File: MeshEditor/src/engine/Time.cpp
#include "engine/Time.h"

namespace MeshEditor
{
    std::chrono::steady_clock::time_point Time::sLast = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point Time::sStart = Time::sLast;
    float Time::sDelta = 0.0f;

    void Time::Tick()
    {
        auto now = std::chrono::steady_clock::now();
        sDelta = std::chrono::duration<float>(now - sLast).count();
        sLast = now;
    }

    float Time::DeltaSeconds()
    {
        return sDelta;
    }

    float Time::TotalSeconds()
    {
        return std::chrono::duration<float>(sLast - sStart).count();
    }
}
