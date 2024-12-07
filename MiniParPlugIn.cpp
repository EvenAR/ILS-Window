#include "pch.h"
#include "MiniParPlugIn.h"

void MiniParPlugIn::OpenNewWindow()
{
    ParWindow* newWindow = new ParWindow();

    if (!newWindow->CreateEx(0, _T("ParWindow"), _T(""), WS_POPUP, 100, 100, 800, 600, nullptr, nullptr)) {
        delete newWindow;
        return;
    }

    newWindow->SetMenu(NULL);
    newWindow->ShowWindow(SW_SHOW);
    newWindow->UpdateWindow();
    newWindow->SetListener(this);

    windows.push_back(newWindow);
}

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

    OpenNewWindow();
    OpenNewWindow();
}

void MiniParPlugIn::OnTimer(int seconds)
{
    EuroScopePlugIn::CPosition runwayThreshold;
    runwayThreshold.m_Latitude = 60.17555659412396;
    runwayThreshold.m_Longitude = 11.107590905118387;

    int runwayElevation = 700;


    ParData parData;
    std::vector<ParTargetPosition> positionHistory;
    positionHistory.push_back({
        1000,
        8,
        0
    });

    parData.radarTargets.push_back({
        "TEST123",
        positionHistory
    });

    for (auto rt = this->RadarTargetSelectFirst(); rt.IsValid(); rt = this->RadarTargetSelectNext(rt)) {
        std::vector<ParTargetPosition> positionHistory;

        EuroScopePlugIn::CRadarTargetPositionData previousPosition;

        for (int i = 0; i <= 9; ++i) { // Get history trail

            EuroScopePlugIn::CRadarTargetPositionData positionData =
                (i == 0) ? rt.GetPosition() : rt.GetPreviousPosition(previousPosition);

            if (!positionData.IsValid()) {
                break;
            }

            positionHistory.push_back({
                positionData.GetPressureAltitude() - runwayElevation,
                positionData.GetPosition().DistanceTo(runwayThreshold),
                positionData.GetPosition().DirectionTo(runwayThreshold)
            });

            previousPosition = positionData;
        }


        parData.radarTargets.push_back({
            rt.GetCallsign(),
            positionHistory
        });

    }

    for (const auto& window : windows) {
        window->SendMessage(WM_UPDATE_DATA, reinterpret_cast<WPARAM>(&parData));
    }

}

void MiniParPlugIn::OnWindowClosed(ParWindow* window)
{
    RemoveWindowFromList(window);
}

void MiniParPlugIn::RemoveWindowFromList(ParWindow* window)
{
    auto it = std::find(windows.begin(), windows.end(), window);
    if (it != windows.end()) {
        windows.erase(it);
    }
}
