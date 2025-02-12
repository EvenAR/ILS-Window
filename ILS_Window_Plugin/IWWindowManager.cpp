#include "pch.h"
#include "IWWindowManager.h"
#include "IWX11Window.h"
#include "IWCdeWindow.h"
#include <algorithm>

IWWindowManager::IWWindowManager(IWSettings* settings)
{
    this->settings = settings;

    AFX_MANAGE_STATE(AfxGetStaticModuleState()); // Manage the module state for MFC

    // Register a custom window class
    WNDCLASS wndClass = { 0 };
    wndClass.lpfnWndProc = ::DefWindowProc;
    wndClass.hInstance = AfxGetInstanceHandle();
    wndClass.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
    wndClass.lpszClassName = WINDOW_CLASS_NAME;

    if (!AfxRegisterClass(&wndClass))
        return;
}

void IWWindowManager::OnWindowClosed(IWWindow* window)
{
    auto it = std::find(windows.begin(), windows.end(), window);
    if (it != windows.end()) {
        windows.erase(it);
    }
}

void IWWindowManager::OnWindowRectangleChanged(IWWindow* window)
{
    CRect windowRect;
    window->GetWindowRect(&windowRect);
    std::string name = window->GetActiveApproachName();
    std::string description = name + " pos.";
    std::string rect = std::to_string(windowRect.left) + "," + std::to_string(windowRect.top) + "," + std::to_string(windowRect.right) + "," + std::to_string(windowRect.bottom);

    this->settings->StoreWindowPositon(name, windowRect);
}

void IWWindowManager::OnToggleThemeClicked(IWWindow* window)
{
    // Close and repoen the window with the other theme
    auto it = std::find(windows.begin(), windows.end(), window);

    if (it != windows.end()) {
        auto activeApproachName = window->GetActiveApproachName();
        auto activeApproach = LookupApproach(activeApproachName);

        auto isX11 = dynamic_cast<IWX11Window*>(window) != nullptr;
        window->DestroyWindow();

        if (isX11) {
            OpenApproachViewWithTheme(activeApproach, IWTheme::CDE);
        }
        else {
            OpenApproachViewWithTheme(activeApproach, IWTheme::X11);
        }
    }
}

IWApproachDefinition IWWindowManager::LookupApproach(std::string approachName)
{
    auto availableApproaches = this->settings->GetAvailableApproaches();
    auto it = std::find_if(availableApproaches.begin(), availableApproaches.end(),
        [&approachName](const IWApproachDefinition& approach) {
            return approach.title == approachName;
        });
    if (it != availableApproaches.end()) {
        return *it;
    }
    return availableApproaches[0];
}

void IWWindowManager::OpenApproachView(IWApproachDefinition approach)
{
    std::string windowStyle = this->settings->GetConfig().behaviour.windowStyle;

    if (windowStyle == "X11") {
        OpenApproachViewWithTheme(approach, IWTheme::X11);
    }
    else {
        OpenApproachViewWithTheme(approach, IWTheme::CDE);
    }
}

void IWWindowManager::OpenApproachViewWithTheme(IWApproachDefinition approach, IWTheme theme)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // Check if a window with the same title is already open
    IWWindow* windowWithSameTitle = nullptr;
    for (const auto& window : windows) {
        if (window->GetActiveApproachName() == approach.title) {
            windowWithSameTitle = window;
            break;
        }
    }

    if (windowWithSameTitle) {
        // Restore the window and bring it to the front
        windowWithSameTitle->ShowWindow(SW_RESTORE);
        windowWithSameTitle->SetForegroundWindow();
        return;
    }

    // Calculate the spawning point for the new window
    CPoint spawningPoint = CPoint(int(windows.size()) * 50, int(windows.size()) * 50 + 100);
    CSize windowSize = CSize(300, 200);

    // Use the saved position if there is one
    auto savedPosition = this->settings->GetWindowPositon(approach.title);
    if (savedPosition) {
        windowSize = savedPosition->Size();
        spawningPoint = savedPosition->TopLeft();
    }
    else {
        // Use the same size as the newest window if there is one.
        IWWindow* newestWindow = windows.size() > 0 ? windows.back() : nullptr;
        if (newestWindow) {
            CRect rect;
            newestWindow->GetWindowRect(&rect);
            windowSize = rect.Size();
            spawningPoint = CPoint(rect.left + 50, rect.top + 50);
        }
    }

    IWWindow* newWindow = nullptr;

    switch (theme) {
    case IWTheme::X11:
        newWindow = new IWX11Window(approach, this->settings->GetConfig().styling);
        break;
    case IWTheme::CDE:
        newWindow = new IWCdeWindow(approach, this->settings->GetConfig().styling);
        break;
    }

    auto hwndPopup = newWindow->CreateEx(
        WS_EX_TOPMOST | WS_EX_APPWINDOW | WS_EX_NOACTIVATE,
        WINDOW_CLASS_NAME,
        _T(approach.title.c_str()),
        WS_POPUP,
        spawningPoint.x,
        spawningPoint.y,
        windowSize.cx,
        windowSize.cy,
        nullptr,
        nullptr
    );

    if (!hwndPopup) {
        delete newWindow;
        return;
    }

    newWindow->SetMenu(NULL);
    newWindow->ShowWindow(SW_SHOWNOACTIVATE); // Show but don't steal focus
    newWindow->UpdateWindow();
    newWindow->SetListener(this);
    newWindow->SetAvailableApproaches(this->settings->GetAvailableApproaches());

    windows.push_back(newWindow);
}

void IWWindowManager::SyncWithActiveRunways(std::vector<IWActiveRunway> activeRunways)
{
    if (!this->settings->GetConfig().behaviour.openWindowsBasedOnActiveRunways) {
        return;
    }

    std::vector<IWApproachDefinition> approachesThatShouldBeOpen;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // Find approaches that should be open
    for (auto& approach : this->settings->GetAvailableApproaches()) {
        auto activeRunway = std::find_if(activeRunways.begin(), activeRunways.end(),
            [&approach](const IWActiveRunway& activeRunway) {
                return activeRunway.airport == approach.airport && activeRunway.runway == approach.runway;
            });
        if (activeRunway != activeRunways.end()) {
            approachesThatShouldBeOpen.push_back(approach);
        }
    }


    // Find open windows that should be closed
    for (auto& window : windows) {
        bool shouldBeClosed = true;
        for (auto& approach : approachesThatShouldBeOpen) {
            if (window->GetActiveApproachName() == approach.title) {
                shouldBeClosed = false;
            }
        }
        if (shouldBeClosed) {
            window->DestroyWindow();
        }
    }

    // Open windows that should be open
    for (auto& approach : approachesThatShouldBeOpen) {
        bool alreadyOpen = std::any_of(windows.begin(), windows.end(), [&approach](const IWWindow* window) {
            return window->GetActiveApproachName() == approach.title;
        });

        if (!alreadyOpen) {
            this->OpenApproachView(approach);
        }
    }
}

bool IWWindowManager::Open(std::string approachTitle)
{
    auto availableApproaches = this->settings->GetAvailableApproaches();

    // Find the approach by title
    auto it = std::find_if(availableApproaches.begin(), availableApproaches.end(),
        [&approachTitle](const IWApproachDefinition& approach) {
            return approach.title == approachTitle;
        });

    if (it != availableApproaches.end()) {
        // Approach found: Open the approach window
        this->OpenApproachView(*it);
        return true; // Command handled
    }
    else {
        return false; // Command not handled
    }
}

void IWWindowManager::HandleLiveData(IWLiveData liveData)
{
    for (auto& window : windows) {
        window->SendMessage(WM_UPDATE_DATA, reinterpret_cast<WPARAM>(&liveData));
    }
}

IWWindowManager::~IWWindowManager()
{
    for (const auto& window : windows) {
        if (window) {
            delete window;
        }
    }
}
