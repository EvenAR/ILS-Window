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

    std::string iniFilePath = GetPluginDirectory() + "//approaches.ini";
    availableApproaches = ReadApproachDefinitions(iniFilePath);
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

    for (auto& app : availableApproaches) {
        OpenNewWindow(&app);
    }
}

MiniParPlugIn::~MiniParPlugIn()
{
    for (const auto& window : windows) {
        if (window) {
            delete window;
        }
    }
}

void MiniParPlugIn::OpenNewWindow(ParApproachDefinition* approach)
{
    bool leftToRight = approach->localizerCourse > 0 && approach->localizerCourse < 180;
    ParWindow* newWindow = new ParWindow(approach->title.c_str(), approach->glideslopeAngle, 25, leftToRight, windowStyling);

    if (!newWindow->CreateEx(0, _T("ParWindow"), _T(""), WS_POPUP, 100, 100, 500, 300, nullptr, nullptr)) {
        delete newWindow;
        return; // Automatically cleaned up when the pointer is deleted
    }

    newWindow->SetMenu(NULL);
    newWindow->ShowWindow(SW_SHOW);
    newWindow->UpdateWindow();
    newWindow->SetListener(this);

    approach->windowReference = newWindow;
    windows.push_back(newWindow);
}

void MiniParPlugIn::OnTimer(int seconds)
{
    for (const auto& approach : availableApproaches) {
        ParData parData;
        if (approach.windowReference == nullptr) continue;

        EuroScopePlugIn::CPosition runwayThreshold;
        runwayThreshold.m_Latitude = approach.thrLatitude;
        runwayThreshold.m_Longitude = approach.thrLongitude;

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
                    positionData.GetPressureAltitude() - approach.thrAltitude,
                    positionData.GetPosition().DistanceTo(runwayThreshold),
                    approach.localizerCourse - positionData.GetPosition().DirectionTo(runwayThreshold)
                    });

                previousPosition = positionData;
            }

            parData.radarTargets.push_back({
                rt.GetCallsign(),
                positionHistory
                });
        }

        approach.windowReference->SendMessage(WM_UPDATE_DATA, reinterpret_cast<WPARAM>(&parData));
    }
}

std::vector<ParApproachDefinition> MiniParPlugIn::ReadApproachDefinitions(const std::string& iniFilePath) {
    std::vector<ParApproachDefinition> approaches;

    // Read all keys from the "Approaches" section
    char keys[1024] = { 0 };
    GetPrivateProfileStringA("Approaches", nullptr, "", keys, sizeof(keys), iniFilePath.c_str());

    char* key = keys;
    while (*key) {
        // Read the value for the current key
        char value[1024] = { 0 };
        GetPrivateProfileStringA("Approaches", key, "", value, sizeof(value), iniFilePath.c_str());

        if (strlen(value) > 0) {
            // Parse the value (format: course,glideslopeAngle,range,thrAltitude,thrLatitude,thrLongitude)
            std::stringstream ss(value);
            ParApproachDefinition approach;

            char comma;
            ss >> approach.localizerCourse >> comma
                >> approach.glideslopeAngle >> comma
                >> approach.range >> comma
                >> approach.thrAltitude >> comma
                >> approach.thrLatitude >> comma
                >> approach.thrLongitude;

            // Set the title as the key name
            approach.title = key;

            // Add to the list
            approaches.push_back(approach);
        }

        // Move to the next key
        key += strlen(key) + 1;
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

std::string MiniParPlugIn::GetPluginDirectory() {
    char modulePath[MAX_PATH];
    GetModuleFileNameA((HINSTANCE)&__ImageBase, modulePath, sizeof(modulePath));
    std::string pluginDirectory = std::string(modulePath).substr(0, std::string(modulePath).find_last_of("\\/"));
    return pluginDirectory;
}

void MiniParPlugIn::OnWindowClosed(ParWindow* window)
{
    // Remove window reference from approach without setting it to nullptr
    for (auto& approach : availableApproaches) {
        if (approach.windowReference == window) {
            approach.windowReference = nullptr;
            break;
        }
    }

    RemoveWindowFromList(window);
}

void MiniParPlugIn::RemoveWindowFromList(ParWindow* window)
{
    auto it = std::find_if(this->windows.begin(), windows.end(),
        [&](ParWindow* w) { return w == window; });

    if (it != windows.end()) {
        windows.erase(it);
    }
}
