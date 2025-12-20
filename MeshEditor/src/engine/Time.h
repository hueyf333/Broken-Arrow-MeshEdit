// File: MeshEditor/src/engine/Time.h
#pragma once

#include <chrono>

namespace MeshEditor
{
    class Time
    {
    public:
        static void Tick();
        static float DeltaSeconds();
        static float TotalSeconds();

    private:
        static std::chrono::steady_clock::time_point sLast;
        static std::chrono::steady_clock::time_point sStart;
        static float sDelta;
    };
}
