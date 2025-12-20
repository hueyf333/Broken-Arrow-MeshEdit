// File: MeshEditor/src/tools/Tool_Bevel.h
#pragma once

#include "tools/Tool.h"

namespace MeshEditor
{
    class Tool_Bevel : public Tool
    {
    public:
        QString Name() const override { return "Bevel"; }
    };
}
