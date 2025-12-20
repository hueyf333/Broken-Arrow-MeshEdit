// File: MeshEditor/src/app/App.h
#pragma once

#include "assets/AssetDatabase.h"
#include "engine/UndoStack.h"
#include "scene/Scene.h"
#include "scene/Selection.h"
#include "tools/ToolManager.h"

namespace MeshEditor
{
    class App
    {
    public:
        App();

        Scene& GetScene();
        Selection& GetSelection();
        UndoStack& GetUndoStack();
        ToolManager& GetToolManager();
        AssetDatabase& GetAssetDatabase();

    private:
        Scene mScene;
        Selection mSelection;
        UndoStack mUndoStack;
        ToolManager mToolManager;
        AssetDatabase mAssets;
    };
}
