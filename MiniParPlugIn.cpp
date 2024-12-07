#include "pch.h"
#include "MiniParPlugIn.h"
#include "IniHelpers.h"



MiniParPlugIn::MiniParPlugIn(void) : CPlugIn(
    EuroScopePlugIn::COMPATIBILITY_CODE,
    "Precision Approach Radar",
    "1.0.0",
    "Gergely Csernak",
    "Free to be distributed as source code"
) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState()); // Manage the module state for MFC

    char modulePath[MAX_PATH];
    GetModuleFileNameA((HINSTANCE)&__ImageBase, modulePath, sizeof(modulePath));
    std::string pluginDirectory = std::string(modulePath).substr(0, std::string(modulePath).find_last_of("\\/"));
    std::string iniFilePath = pluginDirectory + "\\approaches.ini";
    std::vector<ParApproachDefinition> approaches = ReadApproachDefinitions(iniFilePath);

    windowStyling = ReadStyling(iniFilePath);

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

    OpenNewWindow(3.0, 20, true);
    OpenNewWindow(3.0, 25, false);
}

void MiniParPlugIn::OpenNewWindow(double appSlope, double appLength, bool leftToRight)
{
    ParWindow* newWindow = new ParWindow(appSlope, appLength, leftToRight, windowStyling);

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

        for (int i = 0; i <= 9; ++i) { // Create history trail

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

std::vector<ParApproachDefinition> MiniParPlugIn::ReadApproachDefinitions(const std::string& iniFilePath) {
    std::vector<ParApproachDefinition> approaches;
    char section[256];

    // Read the first section (should be non-empty)
    int i = 0;
    while (true) {
        // Construct section name, assuming sections are named like "Approach1", "Approach2", etc.
        sprintf_s(section, sizeof(section), "Approach%d", i + 1);

        // Try reading the section title
        char title[256] = { 0 };
        GetPrivateProfileStringA(section, "title", "", title, sizeof(title), iniFilePath.c_str());

        if (title[0] == '\0') {
            // Stop if we didn't find a valid title (meaning no more sections)
            break;
        }

        // Create a new approach definition
        ParApproachDefinition approach;
        approach.title = title;

        // Read the parameters for the current section (approach)
        approach.thrLatitude = GetIniDouble(section, "thrLatitude", 0.0, iniFilePath);
        approach.thrLongitude = GetIniDouble(section, "thrLongitude", 0.0, iniFilePath);
        approach.thrAltitude = GetIniInt(section, "thrAltitude", 0, iniFilePath);
        approach.trueHeading = (float)GetIniDouble(section, "trueHeading", 0.0, iniFilePath);

        // Add the approach to the list
        approaches.push_back(approach);

        // Increment the index to check the next section
        i++;
    }

    return approaches;
}

ParStyling MiniParPlugIn::ReadStyling(const std::string& iniFilePath)
{
    return ParStyling{
        ReadColorFromIni("Styling", "titleBarBackgroundColor", iniFilePath),
        ReadColorFromIni("Styling", "titleBarTextColor", iniFilePath),
        ReadColorFromIni("Styling", "backgroundColor", iniFilePath),
        ReadColorFromIni("Styling", "glideslopeColor", iniFilePath),
        ReadColorFromIni("Styling", "localizerColor", iniFilePath),
        ReadColorFromIni("Styling", "radarTargetColor", iniFilePath),
        ReadColorFromIni("Styling", "historyTrailColor", iniFilePath),
        ReadColorFromIni("Styling", "targetLabelColor", iniFilePath),
    };
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
