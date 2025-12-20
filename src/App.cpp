#include "App.h"

#include "MeshIO.h"
#include "Primitives.h"
#include "Renderer.h"

#include <commctrl.h>
#include <DirectXMath.h>
#include <cmath>
#include <string>
#include <vector>

namespace {
    constexpr int kPanelMargin = 8;
    constexpr int kSplitterSize = 6;

    enum MenuId {
        ID_FILE_NEW = 100,
        ID_FILE_IMPORT,
        ID_FILE_EXPORT,
        ID_FILE_EXIT,
        ID_EDIT_SUBDIVIDE,
        ID_EDIT_RECALC_NORMALS,
        ID_EDIT_FLIP_NORMALS,
        ID_CREATE_CUBE,
        ID_CREATE_SPHERE,
        ID_CREATE_CYLINDER,
        ID_CREATE_PLANE,
        ID_VIEW_RESET_CAMERA,
        ID_VIEW_TOGGLE_OUTLINER,
        ID_VIEW_TOGGLE_PROPERTIES,
        ID_VIEW_TOGGLE_CONSOLE,
        ID_TOOLS_GIZMO_TRANSLATE,
        ID_TOOLS_GIZMO_ROTATE,
        ID_TOOLS_GIZMO_SCALE,
        ID_TOOLS_SCULPT_MODE,
        ID_HELP_ABOUT,
        ID_PROPERTY_NAME,
        ID_PROPERTY_POSX,
        ID_PROPERTY_POSY,
        ID_PROPERTY_POSZ,
        ID_PROPERTY_ROTX,
        ID_PROPERTY_ROTY,
        ID_PROPERTY_ROTZ,
        ID_PROPERTY_SCLX,
        ID_PROPERTY_SCLY,
        ID_PROPERTY_SCLZ,
        ID_MATERIAL_COLOR,
        ID_MATERIAL_TEXTURE,
        ID_OUTLINER_LIST,
        ID_SCULPT_STRENGTH,
        ID_SCULPT_RADIUS,
    };

    std::wstring OpenFileDialog(HWND owner, const wchar_t *filter) {
        wchar_t filename[MAX_PATH] = {};
        OPENFILENAME ofn = {};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = owner;
        ofn.lpstrFilter = filter;
        ofn.lpstrFile = filename;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
        if (GetOpenFileName(&ofn)) {
            return filename;
        }
        return L"";
    }

    std::wstring SaveFileDialog(HWND owner, const wchar_t *filter) {
        wchar_t filename[MAX_PATH] = {};
        OPENFILENAME ofn = {};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = owner;
        ofn.lpstrFilter = filter;
        ofn.lpstrFile = filename;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_OVERWRITEPROMPT;
        if (GetSaveFileName(&ofn)) {
            return filename;
        }
        return L"";
    }

    void SetFloatText(HWND edit, float value) {
        wchar_t buffer[32];
        swprintf_s(buffer, L"%.3f", value);
        SetWindowText(edit, buffer);
    }

    float GetFloatText(HWND edit) {
        wchar_t buffer[64];
        GetWindowText(edit, buffer, 64);
        return static_cast<float>(_wtof(buffer));
    }
}

App::App(HINSTANCE instance) : m_instance(instance) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    InitCommonControls();
    CreateMainWindow();
    CreateChildWindows();
    CreateMenus();

    m_renderer = std::make_unique<Renderer>(m_viewport);

    auto cube = std::make_unique<MeshObject>();
    cube->name = "Cube";
    cube->mesh = Primitives::MakeCube();
    m_objects.push_back(std::move(cube));
    m_selectedIndex = 0;
    UpdateOutliner();
    UpdatePropertyPanel();
}

App::~App() {
    CoUninitialize();
}

int App::Run() {
    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);

    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            Tick();
        }
    }
    return static_cast<int>(msg.wParam);
}

void App::CreateMainWindow() {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = m_instance;
    wc.lpszClassName = L"BrokenArrowMeshEdit";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClass(&wc);

    m_hwnd = CreateWindowEx(0, wc.lpszClassName, L"Broken Arrow MeshEdit", WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT, 1400, 900, nullptr, nullptr, m_instance, this);
}

void App::CreateChildWindows() {
    WNDCLASS wc = {};
    wc.lpfnWndProc = ViewportProc;
    wc.hInstance = m_instance;
    wc.lpszClassName = L"MeshViewport";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClass(&wc);

    m_viewport = CreateWindowEx(0, wc.lpszClassName, nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER,
                                0, 0, 100, 100, m_hwnd, nullptr, m_instance, this);

    m_leftPanel = CreateWindowEx(0, L"STATIC", nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER,
                                 0, 0, 100, 100, m_hwnd, nullptr, m_instance, nullptr);

    m_rightPanel = CreateWindowEx(0, L"STATIC", nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER,
                                  0, 0, 100, 100, m_hwnd, nullptr, m_instance, nullptr);

    m_bottomPanel = CreateWindowEx(0, L"STATIC", nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER,
                                   0, 0, 100, 100, m_hwnd, nullptr, m_instance, nullptr);

    m_statusBar = CreateWindowEx(0, STATUSCLASSNAME, nullptr, WS_CHILD | WS_VISIBLE,
                                 0, 0, 0, 0, m_hwnd, nullptr, m_instance, nullptr);

    m_outliner = CreateWindowEx(0, L"LISTBOX", nullptr, WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_BORDER,
                                0, 0, 0, 0, m_leftPanel, reinterpret_cast<HMENU>(ID_OUTLINER_LIST), m_instance, nullptr);

    m_propertyName = CreateWindowEx(0, L"EDIT", nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER,
                                    0, 0, 0, 0, m_rightPanel, reinterpret_cast<HMENU>(ID_PROPERTY_NAME), m_instance, nullptr);

    for (int i = 0; i < 3; ++i) {
        m_propertyPos[i] = CreateWindowEx(0, L"EDIT", nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER,
                                          0, 0, 0, 0, m_rightPanel, reinterpret_cast<HMENU>(ID_PROPERTY_POSX + i), m_instance, nullptr);
        m_propertyRot[i] = CreateWindowEx(0, L"EDIT", nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER,
                                          0, 0, 0, 0, m_rightPanel, reinterpret_cast<HMENU>(ID_PROPERTY_ROTX + i), m_instance, nullptr);
        m_propertyScale[i] = CreateWindowEx(0, L"EDIT", nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER,
                                            0, 0, 0, 0, m_rightPanel, reinterpret_cast<HMENU>(ID_PROPERTY_SCLX + i), m_instance, nullptr);
    }

    m_materialColor = CreateWindowEx(0, L"BUTTON", L"Pick Color", WS_CHILD | WS_VISIBLE,
                                     0, 0, 0, 0, m_rightPanel, reinterpret_cast<HMENU>(ID_MATERIAL_COLOR), m_instance, nullptr);

    m_materialTexture = CreateWindowEx(0, L"BUTTON", L"Load Texture", WS_CHILD | WS_VISIBLE,
                                       0, 0, 0, 0, m_rightPanel, reinterpret_cast<HMENU>(ID_MATERIAL_TEXTURE), m_instance, nullptr);

    m_sculptStrength = CreateWindowEx(0, TRACKBAR_CLASS, nullptr, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
                                      0, 0, 0, 0, m_bottomPanel, reinterpret_cast<HMENU>(ID_SCULPT_STRENGTH), m_instance, nullptr);
    SendMessage(m_sculptStrength, TBM_SETRANGE, TRUE, MAKELPARAM(1, 100));
    SendMessage(m_sculptStrength, TBM_SETPOS, TRUE, 35);

    m_sculptRadius = CreateWindowEx(0, TRACKBAR_CLASS, nullptr, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
                                    0, 0, 0, 0, m_bottomPanel, reinterpret_cast<HMENU>(ID_SCULPT_RADIUS), m_instance, nullptr);
    SendMessage(m_sculptRadius, TBM_SETRANGE, TRUE, MAKELPARAM(1, 200));
    SendMessage(m_sculptRadius, TBM_SETPOS, TRUE, 60);

    UpdateLayout();
}

void App::CreateMenus() {
    HMENU menu = CreateMenu();
    HMENU fileMenu = CreatePopupMenu();
    AppendMenu(fileMenu, MF_STRING, ID_FILE_NEW, L"New Scene\tCtrl+N");
    AppendMenu(fileMenu, MF_STRING, ID_FILE_IMPORT, L"Import OBJ...");
    AppendMenu(fileMenu, MF_STRING, ID_FILE_EXPORT, L"Export OBJ...");
    AppendMenu(fileMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenu(fileMenu, MF_STRING, ID_FILE_EXIT, L"Exit");
    AppendMenu(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(fileMenu), L"File");

    HMENU editMenu = CreatePopupMenu();
    AppendMenu(editMenu, MF_STRING, ID_EDIT_SUBDIVIDE, L"Subdivide");
    AppendMenu(editMenu, MF_STRING, ID_EDIT_RECALC_NORMALS, L"Recalculate Normals");
    AppendMenu(editMenu, MF_STRING, ID_EDIT_FLIP_NORMALS, L"Flip Normals");
    AppendMenu(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(editMenu), L"Edit");

    HMENU createMenu = CreatePopupMenu();
    AppendMenu(createMenu, MF_STRING, ID_CREATE_CUBE, L"Cube");
    AppendMenu(createMenu, MF_STRING, ID_CREATE_SPHERE, L"Sphere");
    AppendMenu(createMenu, MF_STRING, ID_CREATE_CYLINDER, L"Cylinder");
    AppendMenu(createMenu, MF_STRING, ID_CREATE_PLANE, L"Plane");
    AppendMenu(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(createMenu), L"Create");

    HMENU viewMenu = CreatePopupMenu();
    AppendMenu(viewMenu, MF_STRING, ID_VIEW_RESET_CAMERA, L"Reset Camera");
    AppendMenu(viewMenu, MF_STRING | MF_CHECKED, ID_VIEW_TOGGLE_OUTLINER, L"Outliner Panel");
    AppendMenu(viewMenu, MF_STRING | MF_CHECKED, ID_VIEW_TOGGLE_PROPERTIES, L"Properties Panel");
    AppendMenu(viewMenu, MF_STRING | MF_CHECKED, ID_VIEW_TOGGLE_CONSOLE, L"Sculpt Panel");
    AppendMenu(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(viewMenu), L"View");

    HMENU toolsMenu = CreatePopupMenu();
    AppendMenu(toolsMenu, MF_STRING, ID_TOOLS_GIZMO_TRANSLATE, L"Gizmo: Translate\tW");
    AppendMenu(toolsMenu, MF_STRING, ID_TOOLS_GIZMO_ROTATE, L"Gizmo: Rotate\tE");
    AppendMenu(toolsMenu, MF_STRING, ID_TOOLS_GIZMO_SCALE, L"Gizmo: Scale\tR");
    AppendMenu(toolsMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenu(toolsMenu, MF_STRING, ID_TOOLS_SCULPT_MODE, L"Toggle Sculpt Mode\tS");
    AppendMenu(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(toolsMenu), L"Tools");

    HMENU helpMenu = CreatePopupMenu();
    AppendMenu(helpMenu, MF_STRING, ID_HELP_ABOUT, L"About");
    AppendMenu(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(helpMenu), L"Help");

    SetMenu(m_hwnd, menu);
}

void App::UpdateLayout() {
    RECT rect;
    GetClientRect(m_hwnd, &rect);
    RECT statusRect;
    SendMessage(m_statusBar, WM_SIZE, 0, 0);
    GetWindowRect(m_statusBar, &statusRect);

    int statusHeight = statusRect.bottom - statusRect.top;
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top - statusHeight;

    int leftWidth = m_leftVisible ? m_layout.leftWidth : 0;
    int rightWidth = m_rightVisible ? m_layout.rightWidth : 0;
    int bottomHeight = m_bottomVisible ? m_layout.bottomHeight : 0;

    int splitterCount = 0;
    if (m_leftVisible) {
        splitterCount++;
    }
    if (m_rightVisible) {
        splitterCount++;
    }
    int horizontalSplitter = m_bottomVisible ? kSplitterSize : 0;

    int viewportWidth = width - leftWidth - rightWidth - splitterCount * kSplitterSize;
    int viewportHeight = height - bottomHeight - horizontalSplitter;

    int x = 0;
    if (m_leftVisible) {
        MoveWindow(m_leftPanel, 0, 0, leftWidth, height - bottomHeight - kSplitterSize, TRUE);
        x += leftWidth + kSplitterSize;
    }

    MoveWindow(m_viewport, x, 0, viewportWidth, viewportHeight, TRUE);

    if (m_rightVisible) {
        MoveWindow(m_rightPanel, x + viewportWidth + kSplitterSize, 0, rightWidth, height - bottomHeight - kSplitterSize, TRUE);
    }

    if (m_bottomVisible) {
        MoveWindow(m_bottomPanel, 0, height - bottomHeight, width, bottomHeight, TRUE);
    }

    if (m_leftVisible) {
        int panelWidth = leftWidth - kPanelMargin * 2;
        MoveWindow(m_outliner, kPanelMargin, kPanelMargin, panelWidth, height - bottomHeight - kSplitterSize - kPanelMargin * 2, TRUE);
    }

    int rightX = kPanelMargin;
    int rightWidthInner = rightWidth - kPanelMargin * 2;
    int y = kPanelMargin;
    if (m_rightVisible) {
        MoveWindow(m_propertyName, rightX, y, rightWidthInner, 24, TRUE);
        y += 32;
        for (int i = 0; i < 3; ++i) {
            MoveWindow(m_propertyPos[i], rightX + i * (rightWidthInner / 3), y, rightWidthInner / 3 - 4, 24, TRUE);
        }
        y += 32;
        for (int i = 0; i < 3; ++i) {
            MoveWindow(m_propertyRot[i], rightX + i * (rightWidthInner / 3), y, rightWidthInner / 3 - 4, 24, TRUE);
        }
        y += 32;
        for (int i = 0; i < 3; ++i) {
            MoveWindow(m_propertyScale[i], rightX + i * (rightWidthInner / 3), y, rightWidthInner / 3 - 4, 24, TRUE);
        }
        y += 36;
        MoveWindow(m_materialColor, rightX, y, rightWidthInner, 28, TRUE);
        y += 36;
        MoveWindow(m_materialTexture, rightX, y, rightWidthInner, 28, TRUE);
    }

    if (m_bottomVisible) {
        int bottomWidth = width - kPanelMargin * 2;
        MoveWindow(m_sculptStrength, kPanelMargin, kPanelMargin, bottomWidth, 30, TRUE);
        MoveWindow(m_sculptRadius, kPanelMargin, kPanelMargin + 36, bottomWidth, 30, TRUE);
    }

    if (m_renderer) {
        RECT viewportRect;
        GetClientRect(m_viewport, &viewportRect);
        m_renderer->Resize(viewportRect.right - viewportRect.left, viewportRect.bottom - viewportRect.top);
    }
}

void App::Tick() {
    if (m_renderer) {
        m_renderer->Render(m_objects, m_selectedIndex, m_camera);
    }
}

void App::OnSize() {
    UpdateLayout();
}

void App::OnCommand(WPARAM wparam) {
    int id = LOWORD(wparam);
    switch (id) {
    case ID_FILE_NEW:
        ResetScene();
        break;
    case ID_FILE_IMPORT:
        ImportMesh();
        break;
    case ID_FILE_EXPORT:
        ExportMesh();
        break;
    case ID_FILE_EXIT:
        PostQuitMessage(0);
        break;
    case ID_EDIT_SUBDIVIDE:
        if (m_selectedIndex >= 0) {
            MeshOps::Subdivide(m_objects[m_selectedIndex]->mesh);
            m_renderer->UpdateMeshBuffers(*m_objects[m_selectedIndex], static_cast<size_t>(m_selectedIndex));
        }
        break;
    case ID_EDIT_RECALC_NORMALS:
        if (m_selectedIndex >= 0) {
            MeshOps::ComputeNormals(m_objects[m_selectedIndex]->mesh);
            m_renderer->UpdateMeshBuffers(*m_objects[m_selectedIndex], static_cast<size_t>(m_selectedIndex));
        }
        break;
    case ID_EDIT_FLIP_NORMALS:
        if (m_selectedIndex >= 0) {
            for (auto &vertex : m_objects[m_selectedIndex]->mesh.vertices) {
                vertex.normal.x *= -1.0f;
                vertex.normal.y *= -1.0f;
                vertex.normal.z *= -1.0f;
            }
            m_renderer->UpdateMeshBuffers(*m_objects[m_selectedIndex], static_cast<size_t>(m_selectedIndex));
        }
        break;
    case ID_CREATE_CUBE:
        AddPrimitive("Cube");
        break;
    case ID_CREATE_SPHERE:
        AddPrimitive("Sphere");
        break;
    case ID_CREATE_CYLINDER:
        AddPrimitive("Cylinder");
        break;
    case ID_CREATE_PLANE:
        AddPrimitive("Plane");
        break;
    case ID_VIEW_RESET_CAMERA:
        m_camera = {};
        break;
    case ID_VIEW_TOGGLE_OUTLINER:
        TogglePanel(m_leftPanel, m_leftVisible);
        CheckMenuItem(GetMenu(m_hwnd), ID_VIEW_TOGGLE_OUTLINER, m_leftVisible ? MF_CHECKED : MF_UNCHECKED);
        break;
    case ID_VIEW_TOGGLE_PROPERTIES:
        TogglePanel(m_rightPanel, m_rightVisible);
        CheckMenuItem(GetMenu(m_hwnd), ID_VIEW_TOGGLE_PROPERTIES, m_rightVisible ? MF_CHECKED : MF_UNCHECKED);
        break;
    case ID_VIEW_TOGGLE_CONSOLE:
        TogglePanel(m_bottomPanel, m_bottomVisible);
        CheckMenuItem(GetMenu(m_hwnd), ID_VIEW_TOGGLE_CONSOLE, m_bottomVisible ? MF_CHECKED : MF_UNCHECKED);
        break;
    case ID_TOOLS_GIZMO_TRANSLATE:
        m_gizmoMode = GizmoMode::Translate;
        break;
    case ID_TOOLS_GIZMO_ROTATE:
        m_gizmoMode = GizmoMode::Rotate;
        break;
    case ID_TOOLS_GIZMO_SCALE:
        m_gizmoMode = GizmoMode::Scale;
        break;
    case ID_TOOLS_SCULPT_MODE:
        m_sculpting = !m_sculpting;
        UpdateStatus(m_sculpting ? L"Sculpt mode enabled" : L"Sculpt mode disabled");
        break;
    case ID_HELP_ABOUT:
        ShowAbout();
        break;
    case ID_MATERIAL_COLOR:
        if (m_selectedIndex >= 0) {
            CHOOSECOLOR cc = {};
            COLORREF custom[16] = {};
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = m_hwnd;
            cc.rgbResult = RGB(230, 230, 230);
            cc.lpCustColors = custom;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;
            if (ChooseColor(&cc)) {
                auto &color = m_objects[m_selectedIndex]->material.baseColor;
                color.x = GetRValue(cc.rgbResult) / 255.0f;
                color.y = GetGValue(cc.rgbResult) / 255.0f;
                color.z = GetBValue(cc.rgbResult) / 255.0f;
                color.w = 1.0f;
            }
        }
        break;
    case ID_MATERIAL_TEXTURE:
        if (m_selectedIndex >= 0) {
            auto path = OpenFileDialog(m_hwnd, L"Image Files\0*.png;*.jpg;*.jpeg\0\0");
            if (!path.empty()) {
                m_objects[m_selectedIndex]->material.texturePath = path;
                m_renderer->UpdateTexture(*m_objects[m_selectedIndex], static_cast<size_t>(m_selectedIndex));
            }
        }
        break;
    case ID_OUTLINER_LIST:
        if (HIWORD(wparam) == LBN_SELCHANGE) {
            m_selectedIndex = static_cast<int>(SendMessage(m_outliner, LB_GETCURSEL, 0, 0));
            UpdatePropertyPanel();
        }
        break;
    default:
        if (id == ID_PROPERTY_NAME && HIWORD(wparam) == EN_CHANGE) {
            if (m_selectedIndex >= 0) {
                char buffer[128] = {};
                GetWindowTextA(m_propertyName, buffer, 128);
                m_objects[m_selectedIndex]->name = buffer;
                UpdateOutliner();
            }
        } else if (id >= ID_PROPERTY_POSX && id <= ID_PROPERTY_SCLZ && HIWORD(wparam) == EN_CHANGE) {
            if (m_selectedIndex >= 0) {
                auto &transform = m_objects[m_selectedIndex]->transform;
                transform.position.x = GetFloatText(m_propertyPos[0]);
                transform.position.y = GetFloatText(m_propertyPos[1]);
                transform.position.z = GetFloatText(m_propertyPos[2]);
                transform.rotation.x = GetFloatText(m_propertyRot[0]);
                transform.rotation.y = GetFloatText(m_propertyRot[1]);
                transform.rotation.z = GetFloatText(m_propertyRot[2]);
                transform.scale.x = GetFloatText(m_propertyScale[0]);
                transform.scale.y = GetFloatText(m_propertyScale[1]);
                transform.scale.z = GetFloatText(m_propertyScale[2]);
            }
        }
        break;
    }
}

void App::OnMouseDown(POINTS pt, bool left) {
    m_lastMouse = {pt.x, pt.y};
    if (left) {
        m_leftDown = true;
    } else {
        m_rightDown = true;
    }
}

void App::OnMouseUp(POINTS, bool left) {
    if (left) {
        m_leftDown = false;
    } else {
        m_rightDown = false;
    }
}

void App::OnMouseMove(POINTS pt) {
    int dx = pt.x - m_lastMouse.x;
    int dy = pt.y - m_lastMouse.y;
    m_lastMouse = {pt.x, pt.y};

    if (m_rightDown) {
        m_camera.position.x += dx * 0.01f;
        m_camera.position.y -= dy * 0.01f;
    }

    if (m_leftDown && m_selectedIndex >= 0) {
        if (m_sculpting) {
            BeginSculpt();
        } else {
            auto &transform = m_objects[m_selectedIndex]->transform;
            float factor = 0.01f;
            if (m_gizmoMode == GizmoMode::Translate) {
                transform.position.x += dx * factor;
                transform.position.y -= dy * factor;
            } else if (m_gizmoMode == GizmoMode::Rotate) {
                transform.rotation.y += dx * factor;
                transform.rotation.x += dy * factor;
            } else if (m_gizmoMode == GizmoMode::Scale) {
                transform.scale.x += dx * factor;
                transform.scale.y += dy * factor;
                transform.scale.z += dx * factor;
            }
            UpdatePropertyPanel();
        }
    }
}

void App::OnMouseWheel(short delta, POINTS) {
    m_camera.position.z += delta > 0 ? 0.5f : -0.5f;
}

void App::OnKeyDown(WPARAM key) {
    if (key == 'W') {
        m_gizmoMode = GizmoMode::Translate;
    } else if (key == 'E') {
        m_gizmoMode = GizmoMode::Rotate;
    } else if (key == 'R') {
        m_gizmoMode = GizmoMode::Scale;
    } else if (key == 'S') {
        m_sculpting = !m_sculpting;
    }
}

void App::TogglePanel(HWND panel, bool &visible) {
    visible = !visible;
    ShowWindow(panel, visible ? SW_SHOW : SW_HIDE);
    UpdateLayout();
}

void App::ImportMesh() {
    auto path = OpenFileDialog(m_hwnd, L"OBJ Mesh\0*.obj\0\0");
    if (path.empty()) {
        return;
    }

    auto meshObject = std::make_unique<MeshObject>();
    meshObject->name = "Imported Mesh";
    std::string error;
    if (!MeshIO::LoadOBJ(path, meshObject->mesh, error)) {
        MessageBoxA(m_hwnd, error.c_str(), "Import Failed", MB_ICONERROR);
        return;
    }

    m_objects.push_back(std::move(meshObject));
    m_selectedIndex = static_cast<int>(m_objects.size() - 1);
    UpdateOutliner();
    UpdatePropertyPanel();
}

void App::ExportMesh() {
    if (m_selectedIndex < 0) {
        return;
    }

    auto path = SaveFileDialog(m_hwnd, L"OBJ Mesh\0*.obj\0\0");
    if (path.empty()) {
        return;
    }

    std::string error;
    if (!MeshIO::SaveOBJ(path, m_objects[m_selectedIndex]->mesh, error)) {
        MessageBoxA(m_hwnd, error.c_str(), "Export Failed", MB_ICONERROR);
    }
}

void App::AddPrimitive(const std::string &type) {
    auto meshObject = std::make_unique<MeshObject>();
    meshObject->name = type;
    if (type == "Cube") {
        meshObject->mesh = Primitives::MakeCube();
    } else if (type == "Sphere") {
        meshObject->mesh = Primitives::MakeSphere();
    } else if (type == "Cylinder") {
        meshObject->mesh = Primitives::MakeCylinder();
    } else if (type == "Plane") {
        meshObject->mesh = Primitives::MakePlane();
    }

    m_objects.push_back(std::move(meshObject));
    m_selectedIndex = static_cast<int>(m_objects.size() - 1);
    m_renderer->UpdateMeshBuffers(*m_objects[m_selectedIndex], static_cast<size_t>(m_selectedIndex));
    UpdateOutliner();
    UpdatePropertyPanel();
}

void App::ResetScene() {
    m_objects.clear();
    auto cube = std::make_unique<MeshObject>();
    cube->name = "Cube";
    cube->mesh = Primitives::MakeCube();
    m_objects.push_back(std::move(cube));
    m_selectedIndex = 0;
    UpdateOutliner();
    UpdatePropertyPanel();
}

void App::ShowAbout() {
    MessageBox(m_hwnd, L"Broken Arrow MeshEdit\nC++20 / DirectX 12", L"About", MB_OK | MB_ICONINFORMATION);
}

void App::UpdatePropertyPanel() {
    if (m_selectedIndex < 0 || m_selectedIndex >= static_cast<int>(m_objects.size())) {
        return;
    }

    auto &object = m_objects[m_selectedIndex];
    SetWindowTextA(m_propertyName, object->name.c_str());

    SetFloatText(m_propertyPos[0], object->transform.position.x);
    SetFloatText(m_propertyPos[1], object->transform.position.y);
    SetFloatText(m_propertyPos[2], object->transform.position.z);

    SetFloatText(m_propertyRot[0], object->transform.rotation.x);
    SetFloatText(m_propertyRot[1], object->transform.rotation.y);
    SetFloatText(m_propertyRot[2], object->transform.rotation.z);

    SetFloatText(m_propertyScale[0], object->transform.scale.x);
    SetFloatText(m_propertyScale[1], object->transform.scale.y);
    SetFloatText(m_propertyScale[2], object->transform.scale.z);
}

void App::UpdateOutliner() {
    SendMessage(m_outliner, LB_RESETCONTENT, 0, 0);
    for (const auto &object : m_objects) {
        SendMessageA(m_outliner, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(object->name.c_str()));
    }
    if (m_selectedIndex >= 0) {
        SendMessage(m_outliner, LB_SETCURSEL, m_selectedIndex, 0);
    }
}

void App::UpdateStatus(const std::wstring &text) {
    SendMessage(m_statusBar, SB_SETTEXT, 0, reinterpret_cast<LPARAM>(text.c_str()));
}

void App::BeginSculpt() {
    if (m_selectedIndex < 0) {
        return;
    }
    auto &mesh = m_objects[m_selectedIndex]->mesh;
    if (mesh.vertices.empty()) {
        return;
    }

    RECT rect;
    GetClientRect(m_viewport, &rect);
    float width = static_cast<float>(rect.right - rect.left);
    float height = static_cast<float>(rect.bottom - rect.top);

    using namespace DirectX;
    XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&m_camera.position), XMLoadFloat3(&m_camera.target), XMVectorSet(0, 1, 0, 0));
    XMMATRIX proj = XMMatrixPerspectiveFovLH(m_camera.fov, width / height, m_camera.nearPlane, m_camera.farPlane);

    float radius = static_cast<float>(SendMessage(m_sculptRadius, TBM_GETPOS, 0, 0));
    float strength = static_cast<float>(SendMessage(m_sculptStrength, TBM_GETPOS, 0, 0)) * 0.0005f;

    POINT cursor;
    GetCursorPos(&cursor);
    ScreenToClient(m_viewport, &cursor);

    for (auto &vertex : mesh.vertices) {
        XMVECTOR pos = XMLoadFloat3(&vertex.position);
        XMVECTOR clip = XMVector3TransformCoord(pos, view * proj);
        XMFLOAT3 clipPos;
        XMStoreFloat3(&clipPos, clip);
        float sx = (clipPos.x * 0.5f + 0.5f) * width;
        float sy = (-clipPos.y * 0.5f + 0.5f) * height;
        float dx = sx - cursor.x;
        float dy = sy - cursor.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist < radius) {
            vertex.position.x += vertex.normal.x * strength;
            vertex.position.y += vertex.normal.y * strength;
            vertex.position.z += vertex.normal.z * strength;
        }
    }

    m_renderer->UpdateMeshBuffers(*m_objects[m_selectedIndex], static_cast<size_t>(m_selectedIndex));
}

void App::EndSculpt() {
    m_sculpting = false;
}

LRESULT CALLBACK App::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    App *app = reinterpret_cast<App *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (msg == WM_NCCREATE) {
        auto create = reinterpret_cast<CREATESTRUCT *>(lparam);
        app = reinterpret_cast<App *>(create->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
    }

    if (!app) {
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    switch (msg) {
    case WM_LBUTTONDOWN: {
        POINTS pt = MAKEPOINTS(lparam);
        RECT rect;
        GetClientRect(hwnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        int leftEdge = app->m_leftVisible ? app->m_layout.leftWidth : 0;
        int rightEdge = app->m_rightVisible ? width - app->m_layout.rightWidth : width;
        int bottomEdge = app->m_bottomVisible ? height - app->m_layout.bottomHeight : height;

        if (app->m_leftVisible && pt.x >= leftEdge && pt.x <= leftEdge + kSplitterSize) {
            app->m_draggingLeft = true;
            SetCapture(hwnd);
        } else if (app->m_rightVisible && pt.x >= rightEdge - kSplitterSize && pt.x <= rightEdge) {
            app->m_draggingRight = true;
            SetCapture(hwnd);
        } else if (app->m_bottomVisible && pt.y >= bottomEdge - kSplitterSize && pt.y <= bottomEdge) {
            app->m_draggingBottom = true;
            SetCapture(hwnd);
        }
        return 0;
    }
    case WM_LBUTTONUP:
        app->m_draggingLeft = false;
        app->m_draggingRight = false;
        app->m_draggingBottom = false;
        ReleaseCapture();
        return 0;
    case WM_MOUSEMOVE: {
        if (app->m_draggingLeft || app->m_draggingRight || app->m_draggingBottom) {
            POINTS pt = MAKEPOINTS(lparam);
            RECT rect;
            GetClientRect(hwnd, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            if (app->m_draggingLeft) {
                app->m_layout.leftWidth = max(160, min(pt.x, width - 300));
            }
            if (app->m_draggingRight) {
                app->m_layout.rightWidth = max(240, min(width - pt.x, width - 200));
            }
            if (app->m_draggingBottom) {
                app->m_layout.bottomHeight = max(120, min(height - pt.y, height - 120));
            }
            app->UpdateLayout();
        }
        return 0;
    }
    case WM_SIZE:
        app->OnSize();
        return 0;
    case WM_COMMAND:
        app->OnCommand(wparam);
        return 0;
    case WM_KEYDOWN:
        app->OnKeyDown(wparam);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }
}

LRESULT CALLBACK App::ViewportProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    App *app = reinterpret_cast<App *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (msg == WM_NCCREATE) {
        auto create = reinterpret_cast<CREATESTRUCT *>(lparam);
        app = reinterpret_cast<App *>(create->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
    }

    if (!app) {
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    switch (msg) {
    case WM_LBUTTONDOWN:
        SetCapture(hwnd);
        app->OnMouseDown(MAKEPOINTS(lparam), true);
        return 0;
    case WM_LBUTTONUP:
        ReleaseCapture();
        app->OnMouseUp(MAKEPOINTS(lparam), true);
        return 0;
    case WM_RBUTTONDOWN:
        SetCapture(hwnd);
        app->OnMouseDown(MAKEPOINTS(lparam), false);
        return 0;
    case WM_RBUTTONUP:
        ReleaseCapture();
        app->OnMouseUp(MAKEPOINTS(lparam), false);
        return 0;
    case WM_MOUSEMOVE:
        app->OnMouseMove(MAKEPOINTS(lparam));
        return 0;
    case WM_MOUSEWHEEL:
        app->OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wparam), MAKEPOINTS(lparam));
        return 0;
    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }
}
