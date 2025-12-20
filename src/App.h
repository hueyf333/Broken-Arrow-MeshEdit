#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <memory>

#include "Renderer.h"

struct MeshObject;
class Renderer;

struct PanelLayout {
    int leftWidth = 240;
    int rightWidth = 300;
    int bottomHeight = 200;
};

enum class GizmoMode {
    Translate,
    Rotate,
    Scale,
};

class App {
public:
    App(HINSTANCE instance);
    ~App();

    int Run();

private:
    void CreateMainWindow();
    void CreateChildWindows();
    void CreateMenus();
    void UpdateLayout();

    void OnSize();
    void OnCommand(WPARAM wparam);
    void OnMouseDown(POINTS pt, bool left);
    void OnMouseUp(POINTS pt, bool left);
    void OnMouseMove(POINTS pt);
    void OnMouseWheel(short delta, POINTS pt);
    void OnKeyDown(WPARAM key);

    void TogglePanel(HWND panel, bool &visible);

    void ImportMesh();
    void ExportMesh();
    void AddPrimitive(const std::string &type);
    void ResetScene();
    void ShowAbout();

    void UpdatePropertyPanel();
    void UpdateOutliner();
    void UpdateStatus(const std::wstring &text);

    void BeginSculpt();
    void EndSculpt();

    void Tick();

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    static LRESULT CALLBACK ViewportProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    HINSTANCE m_instance = nullptr;
    HWND m_hwnd = nullptr;
    HWND m_viewport = nullptr;
    HWND m_leftPanel = nullptr;
    HWND m_rightPanel = nullptr;
    HWND m_bottomPanel = nullptr;
    HWND m_statusBar = nullptr;

    HWND m_outliner = nullptr;
    HWND m_propertyName = nullptr;
    HWND m_propertyPos[3] = {};
    HWND m_propertyRot[3] = {};
    HWND m_propertyScale[3] = {};
    HWND m_materialColor = nullptr;
    HWND m_materialTexture = nullptr;
    HWND m_sculptStrength = nullptr;
    HWND m_sculptRadius = nullptr;

    bool m_leftVisible = true;
    bool m_rightVisible = true;
    bool m_bottomVisible = true;

    PanelLayout m_layout = {};
    bool m_draggingLeft = false;
    bool m_draggingRight = false;
    bool m_draggingBottom = false;

    std::unique_ptr<Renderer> m_renderer;
    std::vector<std::unique_ptr<MeshObject>> m_objects;
    int m_selectedIndex = -1;
    GizmoMode m_gizmoMode = GizmoMode::Translate;
    bool m_sculpting = false;
    Camera m_camera = {};

    POINT m_lastMouse = {};
    bool m_leftDown = false;
    bool m_rightDown = false;
};
