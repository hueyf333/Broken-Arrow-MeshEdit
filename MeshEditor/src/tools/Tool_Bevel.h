// File: MeshEditor/src/tools/Tool_Bevel.h
#pragma once

#include "Tool.h"

namespace MeshEditor
{
class Tool_Bevel : public Tool
{
public:
    std::string GetName() const override { return "Bevel"; }
};
}
