// File: MeshEditor/src/tools/Tool_Bevel.h
#pragma once

#include "Tool.h"

namespace mesh
{
class Tool_Bevel : public Tool
{
public:
    ToolType Type() const override { return ToolType::Bevel; }
    QString Name() const override { return QStringLiteral("Bevel"); }
};
}
