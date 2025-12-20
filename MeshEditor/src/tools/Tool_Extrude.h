// File: MeshEditor/src/tools/Tool_Extrude.h
#pragma once

#include "tools/Tool.h"

namespace MeshEditor
{
    class Tool_Extrude : public Tool
    {
    public:
        QString Name() const override { return "Extrude"; }
    };
}
