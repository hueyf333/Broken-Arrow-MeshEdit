// File: MeshEditor/src/app/App.cpp
#include "app/App.h"
#include "tools/Tool_Select.h"
#include "tools/Tool_Transform.h"

namespace MeshEditor
{
    App::App()
    {
        mToolManager.Register(std::make_unique<Tool_Select>());
        mToolManager.Register(std::make_unique<Tool_Transform>(TransformMode::Move));
        mToolManager.Register(std::make_unique<Tool_Transform>(TransformMode::Rotate));
        mToolManager.Register(std::make_unique<Tool_Transform>(TransformMode::Scale));
    }

    Scene& App::GetScene()
    {
        return mScene;
    }

    Selection& App::GetSelection()
    {
        return mSelection;
    }

    UndoStack& App::GetUndoStack()
    {
        return mUndoStack;
    }

    ToolManager& App::GetToolManager()
    {
        return mToolManager;
    }

    AssetDatabase& App::GetAssetDatabase()
    {
        return mAssets;
    }
}
