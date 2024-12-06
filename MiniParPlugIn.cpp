#include "pch.h"
#include "MiniParPlugIn.h"

MiniParPlugIn::MiniParPlugIn(void) : CPlugIn(
    EuroScopePlugIn::COMPATIBILITY_CODE,
    "Precision Approach Radar",
    "1.0.0",
    "Gergely Csernak",
    "Free to be distributed as source code"
) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState()); // Manage the module state for MFC

    // Register a custom window class if not already done
    static bool isRegistered = false;
    if (!isRegistered)
    {
        WNDCLASS wndClass = { 0 };
        wndClass.lpfnWndProc = ::DefWindowProc;
        wndClass.hInstance = AfxGetInstanceHandle();
        wndClass.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
        wndClass.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
        wndClass.lpszClassName = _T("ParWindow");

        if (!AfxRegisterClass(&wndClass))
            return;

        isRegistered = true;
    }

    // Create the window
    ParWindow* pCanvas = new ParWindow();

    if (!pCanvas->CreateEx(0, _T("ParWindow"), _T(""), WS_POPUP, 100, 100, 800, 600, nullptr, nullptr)) {
        delete pCanvas;
        return;
    }

    pCanvas->SetMenu(NULL);
    pCanvas->ShowWindow(SW_SHOW);
    pCanvas->UpdateWindow();
}