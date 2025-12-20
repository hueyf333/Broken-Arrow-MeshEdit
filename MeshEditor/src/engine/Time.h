// File: MeshEditor/src/engine/Time.h
#pragma once

namespace mesh
{
class Time
{
public:
    static void Tick();
    static float DeltaSeconds();
    static float TotalSeconds();
};
}
