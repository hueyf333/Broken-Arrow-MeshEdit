// File: MeshEditor/src/engine/Time.h
#pragma once

#include <chrono>

namespace MeshEditor
{
class Time
{
public:
    static float DeltaTime();
    static void Tick();

private:
    static std::chrono::steady_clock::time_point s_last;
    static float s_delta;
};
}
