// File: MeshEditor/src/tools/Tool_Select.h
#pragma once

#include "Tool.h"

namespace MeshEditor
{
class Tool_Select : public Tool
{
public:
    std::string GetName() const override { return "Select"; }
    void OnMousePress(QMouseEvent* event, Scene* scene) override;
};
}
