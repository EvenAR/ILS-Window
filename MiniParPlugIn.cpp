#include "pch.h"
#include "MiniParPlugIn.h"
#include <json.hpp>
#include <fstream>
#include "Utils.h"
#include <regex>

using json = nlohmann::json;

MiniParPlugIn::MiniParPlugIn(void) : CPlugIn(
    EuroScopePlugIn::COMPATIBILITY_CODE,
    "Precision Approach Radar",
    "1.0.0",
    "Even Rognlien",
    "Free to be distributed as source code"
) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState()); // Manage the module state for MFC
    
    // Read configuration file
    std::string jsonFilePath = GetPluginDirectory() + "//PAR.json";
    availableApproaches = ReadApproachDefinitions(jsonFilePath);
    windowStyling = ReadStyling(jsonFilePath);
    behaviourSettings = ReadBehaviourSettings(jsonFilePath);

    // Register a custom window class
    WNDCLASS wndClass = { 0 };
    wndClass.lpfnWndProc = ::DefWindowProc;
    wndClass.hInstance = AfxGetInstanceHandle();
    wndClass.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
    wndClass.lpszClassName = _T("ParWindow");

    if (!AfxRegisterClass(&wndClass))
        return;

    SyncWithActiveRunways();
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
    if (approach->windowReference) return; // Already an open window for this approach

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool leftToRight = approach->localizerCourse > 0 && approach->localizerCourse < 180;
    ParWindow* newWindow = new ParWindow(
        approach->title.c_str(), 
        approach->glideslopeAngle, 
        approach->defaultRange, 
        leftToRight,
        approach->maxOffsetLeft, 
        approach->maxOffsetRight,
        windowStyling
    );

    if (!newWindow->CreateEx(0, _T("ParWindow"), _T(approach->title.c_str()), WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, 500, 300, nullptr, nullptr)) {
        delete newWindow;
        return; // Automatically cleaned up when the pointer is deleted
    }

    newWindow->SetMenu(NULL);
    newWindow->ShowWindow(SW_SHOW);
    newWindow->UpdateWindow();
    newWindow->SetListener(this);
    newWindow->SetForegroundWindow();

    approach->windowReference = newWindow;
    windows.push_back(newWindow);
}


void MiniParPlugIn::OnTimer(int seconds)
{
    for (const auto& approach : availableApproaches) {
        ParData parData;
        if (approach.windowReference == nullptr) continue;

        EuroScopePlugIn::CPosition runwayThreshold;
        runwayThreshold.m_Latitude = approach.thresholdLatitude;
        runwayThreshold.m_Longitude = approach.thresholdLongitude;

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
                    positionData.GetPressureAltitude() - approach.thresholdAltitude,
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

void MiniParPlugIn::OnAirportRunwayActivityChanged()
{
    SyncWithActiveRunways();
}


void MiniParPlugIn::SyncWithActiveRunways()
{
    if (!behaviourSettings.openWindowsBasedOnActiveRunways) {
        return;
    }

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool forArrival = true;

    std::vector<ParApproachDefinition*> approachesThatShouldBeOpen;

    for (auto airport = this->SectorFileElementSelectFirst(EuroScopePlugIn::SECTOR_ELEMENT_AIRPORT); airport.IsValid(); airport = this->SectorFileElementSelectNext(airport, EuroScopePlugIn::SECTOR_ELEMENT_AIRPORT)) {
        if (airport.IsElementActive(!forArrival)) {
            auto activeAirportIcao = std::string(airport.GetName());
            for (auto runway = this->SectorFileElementSelectFirst(EuroScopePlugIn::SECTOR_ELEMENT_RUNWAY); runway.IsValid(); runway = this->SectorFileElementSelectNext(runway, EuroScopePlugIn::SECTOR_ELEMENT_RUNWAY)) {
                auto runwayAirportName = trimString(std::string(runway.GetAirportName()));
                if (runwayAirportName == activeAirportIcao) {
                    for (int runwayDirection = 0; runwayDirection < 2; runwayDirection++) {
                        if (runway.IsElementActive(!forArrival, runwayDirection) && runwayAirportName == activeAirportIcao) {
                            auto runwayName = trimString(std::string(runway.GetRunwayName(runwayDirection)));
                            auto approach = std::find_if(this->availableApproaches.begin(), this->availableApproaches.end(),
                                [&runwayAirportName, runwayName](const ParApproachDefinition& approach) {
                                    return approach.airport == runwayAirportName && approach.runway == runwayName;
                                });

                            if (approach != this->availableApproaches.end()) {
                                approachesThatShouldBeOpen.push_back(&(*approach));
                            }
                        }
                    }
                }
            }
        }
    }

    for (auto& window : windows) {
        // Find windows that are not in the updated list of appraoches that should be open
        bool shouldBeClosed = true;
        for (auto& approach : approachesThatShouldBeOpen) {
            if (approach->windowReference == window) {
                shouldBeClosed = false;
            }
        }
        if (shouldBeClosed) {
            window->DestroyWindow();
        }
    }

    // Open windows that should be open
    for (auto& approach : approachesThatShouldBeOpen) {
        if (approach->windowReference == nullptr) {
            this->OpenNewWindow(approach);
        }
    }
}

std::vector<ParApproachDefinition> MiniParPlugIn::ReadApproachDefinitions(const std::string& jsonFilePath) {
    std::vector<ParApproachDefinition> approaches;

    // Open the JSON file
    std::ifstream file(jsonFilePath);
    if (!file.is_open()) {
        this->DisplayUserMessage("PAR plugin", "Error", (std::string("Unable to open JSON file: ") + jsonFilePath).c_str(), false, true, false, false, false);
        return approaches;
    }

    // Parse the JSON file
    nlohmann::json jsonData;
    try {
        file >> jsonData;
    }
    catch (const nlohmann::json::parse_error& e) {
        this->DisplayUserMessage("PAR plugin", "Error", ("JSON parsing error: " + std::string(e.what())).c_str(), false, true, false, false, false);
        return approaches;
    }

    // Check if "approaches" key exists
    if (!jsonData.contains("approaches") || !jsonData["approaches"].is_array()) {
        this->DisplayUserMessage("PAR plugin", "Error", "'approaches' key not found or is not an array.", false, true, false, false, false);
        return approaches;
    }

    // Iterate over the approaches
    for (const auto& approachJson : jsonData["approaches"]) {
        try {
            ParApproachDefinition approach;

            // Parse individual fields
            approach.title = approachJson.at("title").get<std::string>();
            approach.airport = approachJson.at("airport").get<std::string>();
            approach.runway = approachJson.at("runway").get<std::string>();
            approach.localizerCourse = approachJson.at("localizerCourse").get<int>();
            approach.glideslopeAngle = approachJson.at("glideslopeAngle").get<float>();
            approach.defaultRange = approachJson.at("defaultRange").get<int>();
            approach.thresholdAltitude = approachJson.at("thresholdAltitude").get<int>();
            approach.thresholdLatitude = approachJson.at("thresholdLatitude").get<double>();
            approach.thresholdLongitude = approachJson.at("thresholdLongitude").get<double>();
            approach.maxOffsetLeft = approachJson.at("maxOffsetLeft").get<double>();
            approach.maxOffsetRight = approachJson.at("maxOffsetRight").get<double>();


            // Add to the list
            approaches.push_back(approach);
        }
        catch (const nlohmann::json::exception& e) {
            this->DisplayUserMessage("PAR plugin", "Error", ("Error parsing approach data: " + std::string(e.what())).c_str(), false, true, false, false, false);
        }
    }

    return approaches;
}

ParStyling MiniParPlugIn::ReadStyling(const std::string& jsonFilePath) {
    std::ifstream file(jsonFilePath);
    if (!file.is_open()) {
        this->DisplayUserMessage("PAR plugin", "Error", (std::string("Unable to open JSON file: ") + jsonFilePath).c_str(), false, true, false, false, false);
    }

    nlohmann::json jsonData;
    file >> jsonData;

    auto readColor = [&jsonData,this](const std::string& key) -> RGB {
        if (jsonData.contains("styling") && jsonData["styling"].contains(key)) {
            return HexToRGB(jsonData["styling"][key].get<std::string>());
        }
         this->DisplayUserMessage("PAR plugin", "Error", "'approaches' key not found or is not an array.", false, true, false, false, false);
        };

    return ParStyling{
        readColor("windowFrameColor"),
        readColor("windowFrameTextColor"),
        readColor("backgroundColor"),
        readColor("glideslopeColor"),
        readColor("localizerColor"),
        readColor("radarTargetColor"),
        readColor("historyTrailColor"),
        readColor("targetLabelColor"),
        readColor("rangeStatusTextColor"),
    };
}

ParBehaviourSettings MiniParPlugIn::ReadBehaviourSettings(const std::string& jsonFilePath) {
    std::ifstream file(jsonFilePath);
    if (!file.is_open()) {
        this->DisplayUserMessage("PAR plugin", "Error", (std::string("Unable to open JSON file: ") + jsonFilePath).c_str(), false, true, false, false, false);
    }

    nlohmann::json jsonData;
    file >> jsonData;

    nlohmann::json behaviourSettings = jsonData["behaviour"];

    return ParBehaviourSettings{
        behaviourSettings.at("openWindowsBasedOnActiveRunways").get<bool>()
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
    auto it = std::find(windows.begin(), windows.end(), window);
    if (it != windows.end()) {
        windows.erase(it);
    }

    // Remove window reference from approach without setting it to nullptr
    for (auto& approach : availableApproaches) {
        if (approach.windowReference == window) {
            approach.windowReference = nullptr;
        }
    }
}

bool MiniParPlugIn::OnCompileCommand(const char* sCommandLine)
{
    const std::string command(sCommandLine);
    const std::string prefix = ".par ";

    if (command.rfind(prefix, 0) != 0) { // Command must start with ".par "
        return false; // Not handled
    }

    // Extract the argument after ".par "
    std::string argument = stringToUpper(trimString(command.substr(prefix.length())));

    if (argument.empty()) {
        this->DisplayUserMessage("PAR plugin", "Error", "No approach name specified after '.par'.", false, true, false, false, false);
        return false;
    }

    // Find the approach by title
    auto it = std::find_if(this->availableApproaches.begin(), this->availableApproaches.end(),
        [&argument](const ParApproachDefinition& approach) {
            return approach.title == argument;
        });

    if (it != this->availableApproaches.end()) {
        // Approach found: Open the approach window
        if (it->windowReference == nullptr) {
            this->OpenNewWindow(&(*it));
        }
        return true; // Command handled
    }
    else {
        // Approach not found
        std::string errorMessage = "Approach '" + argument + "' not found.";
        this->DisplayUserMessage("PAR plugin", "Error", errorMessage.c_str(), false, true, false, false, false);
        return false; // Command not handled
    }
}
