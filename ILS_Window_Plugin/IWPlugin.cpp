#include "pch.h"
#include "IWPlugin.h"
#include <json.hpp>
#include <fstream>
#include "IWUtils.h"
#include <regex>

using json = nlohmann::json;

IWPlugin::IWPlugin(void) : CPlugIn(EuroScopePlugIn::COMPATIBILITY_CODE, MY_PLUGIN_NAME, MY_PLUGIN_VERSION, MY_PLUGIN_DEVELOPER, MY_PLUGIN_COPYRIGHT) {

    AFX_MANAGE_STATE(AfxGetStaticModuleState()); // Manage the module state for MFC
    
    // Read configuration file
    std::string jsonFilePath = GetPluginDirectory() + "\\" + CONFIG_FILE_NAME;
    availableApproaches = ReadApproachDefinitions(jsonFilePath);
    windowStyling = ReadStyling(jsonFilePath);
    behaviourSettings = ReadBehaviourSettings(jsonFilePath);

    // Register a custom window class
    WNDCLASS wndClass = { 0 };
    wndClass.lpfnWndProc = ::DefWindowProc;
    wndClass.hInstance = AfxGetInstanceHandle();
    wndClass.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
    wndClass.lpszClassName = WINDOW_CLASS_NAME;

    if (!AfxRegisterClass(&wndClass))
        return;

    LoadSavedWindowPositions();
    SyncWithActiveRunways();
}

IWPlugin::~IWPlugin()
{
    for (const auto& window : windows) {
        if (window) {
            delete window;
        }
    }
}

void IWPlugin::OpenNewWindow(IWApproachDefinition* approach)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CPoint spawningPoint = CPoint(int(windows.size()) * 50, int(windows.size()) * 50 + 100);
    CSize windowSize = CSize(300, 200);

    // Use the saved position if there is one
    auto savedPosition = savedWindowPositions.find(approach->title);
    if (savedPosition != savedWindowPositions.end()) {
        windowSize = savedPosition->second.Size();
        spawningPoint = savedPosition->second.TopLeft();
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
   
    IWWindow* newWindow = new IWWindow(*approach, windowStyling);
    auto hwndPopup = newWindow->CreateEx(
        WS_EX_NOACTIVATE | WS_EX_TOPMOST,
        WINDOW_CLASS_NAME,
        _T(approach->title.c_str()),
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
    newWindow->SetAvailableApproaches(availableApproaches);

    windows.push_back(newWindow);
}


void IWPlugin::OnTimer(int seconds)
{
    IWLiveData liveData;

    for (auto rt = this->RadarTargetSelectFirst(); rt.IsValid(); rt = this->RadarTargetSelectNext(rt)) {
        std::vector<IWTargetPosition> positionHistory;

        EuroScopePlugIn::CRadarTargetPositionData previousPosition;

        for (int i = 0; i <= 9; ++i) { // Last 10 positions (for history trail)

            EuroScopePlugIn::CRadarTargetPositionData positionData =
                (i == 0) ? rt.GetPosition() : rt.GetPreviousPosition(previousPosition);

            const bool notOfInterest =
                positionData.GetReportedGS() < 30 || 
                positionData.GetPressureAltitude() > 10000;

            if (notOfInterest || !positionData.IsValid()) {
                break;
            }

            positionHistory.push_back({
                positionData.GetPressureAltitude(),
                positionData.GetPosition().m_Latitude,
                positionData.GetPosition().m_Longitude,
            });

            previousPosition = positionData;
        }

        auto correlatedFlightPlan = rt.GetCorrelatedFlightPlan();

        std::string aircraftIcaoType = correlatedFlightPlan.IsValid()
            ? std::string(correlatedFlightPlan.GetFlightPlanData().GetAircraftFPType())
            : std::string("");

        char aircraftWtc = correlatedFlightPlan.IsValid()
            ? correlatedFlightPlan.GetFlightPlanData().GetAircraftWtc()
            : ' ';

        if (positionHistory.size() > 0) {
            liveData.radarTargets.push_back({
                rt.GetCallsign(),
                rt.GetPosition().GetSquawk(),
                aircraftIcaoType,
                aircraftWtc,
                positionHistory
            });
            liveData.airportTemperatures = airportTemperatures;
        }
    }

    for (auto& window : windows) {
        window->SendMessage(WM_UPDATE_DATA, reinterpret_cast<WPARAM>(&liveData));
    }
}

void IWPlugin::OnAirportRunwayActivityChanged()
{
    SyncWithActiveRunways();
}

void IWPlugin::OnNewMetarReceived(const char* sStation, const char* sFullMetar)
{
    // Extract temperature from METAR to be used for altitude correction
    std::string station = std::string(sStation);
    std::string fullMetar = std::string(sFullMetar);
    std::regex temperatureRegex(R"((M?\d{2})\/(M?\d{2}))");
    std::smatch temperatureMatch;
    if (std::regex_search(fullMetar, temperatureMatch, temperatureRegex)) {
        std::string temperatureString = temperatureMatch[1];
        bool isNegative = temperatureString[0] == 'M';
        int temperature = std::stoi(temperatureString.substr(1));
        if (isNegative) {
            temperature = -temperature;
        }
        airportTemperatures[station] = temperature;
    }
}

void IWPlugin::SyncWithActiveRunways()
{
    if (!behaviourSettings.openWindowsBasedOnActiveRunways) {
        return;
    }

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool forArrival = true;

    std::vector<IWApproachDefinition*> approachesThatShouldBeOpen;

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
                                [&runwayAirportName, runwayName](const IWApproachDefinition& approach) {
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

    // Find open windows that should be closed
    for (auto& window : windows) {
        bool shouldBeClosed = true;
        for (auto& approach : approachesThatShouldBeOpen) {
            if (window->GetActiveApproachName() == approach->title) {
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
            return window->GetActiveApproachName() == approach->title;
        }); 

        if (!alreadyOpen) {
            this->OpenNewWindow(approach);
        }
    }
}

std::vector<IWApproachDefinition> IWPlugin::ReadApproachDefinitions(const std::string& jsonFilePath) {
    const std::string generalErrorMessage = "Could not load approach definitions";

    std::vector<IWApproachDefinition> approaches;

    // Open the JSON file
    std::ifstream file(jsonFilePath);
    if (!file.is_open()) {
        this->ShowErrorMessage(generalErrorMessage, "Unable to open JSON file '" + jsonFilePath + "'");
        return approaches;
    }

    // Parse the JSON file
    nlohmann::json jsonData;
    try {
        file >> jsonData;
    }
    catch (const nlohmann::json::parse_error& e) {
        this->ShowErrorMessage(generalErrorMessage, std::string(e.what()));
        return approaches;
    }

    // Check if "approaches" key exists
    if (!jsonData.contains("approaches") || !jsonData["approaches"].is_array()) {
        this->ShowErrorMessage(generalErrorMessage, "'approaches' key not found or is not an array.");
        return approaches;
    }

    // Iterate over the approaches
    for (const auto& approachJson : jsonData["approaches"]) {
        try {
            IWApproachDefinition approach;

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
            this->ShowErrorMessage(generalErrorMessage, "Error parsing approach data: " + std::string(e.what()));
        }
    }

    return approaches;
}

IWStyling IWPlugin::ReadStyling(const std::string& jsonFilePath) {
    const std::string generalErrorMessage = "Could not load style settings";

    std::ifstream file(jsonFilePath);
    if (!file.is_open()) {
        this->ShowErrorMessage(generalErrorMessage, "Unable to open JSON file: " + jsonFilePath);
    }

    nlohmann::json jsonData;
    file >> jsonData;

    auto readColor = [&jsonData, &generalErrorMessage, this](const std::string& key) -> RGB {
        if (jsonData.contains("styling") && jsonData["styling"].contains(key)) {
            return HexToRGB(jsonData["styling"][key].get<std::string>());
        }
        this->ShowErrorMessage(generalErrorMessage, ("'" + key + "' key not found.").c_str());
    };

    auto readUnsignedInt = [&jsonData, &generalErrorMessage, this](const std::string& key) -> unsigned int {
        if (jsonData.contains("styling") && jsonData["styling"].contains(key)) {
            return jsonData["styling"][key].get<unsigned int>();
        }
        this->ShowErrorMessage(generalErrorMessage, ("'" + key + "' key not found.").c_str());
        return 0; // Default value when key is not found or is not an unsigned integer
    };

    auto readUIntWithDefault = [&jsonData, this](const std::string& key, unsigned int defaultValue) -> unsigned int {
        if (jsonData.contains("styling") && jsonData["styling"].contains(key)) {
            return jsonData["styling"][key].get<unsigned int>();
        }
        return defaultValue;
    };

    auto readStringWithDefault = [&jsonData, this](const std::string& key, const std::string& defaultValue) -> std::string {
        if (jsonData.contains("styling") && jsonData["styling"].contains(key)) {
            return jsonData["styling"][key].get<std::string>();
        }
        return defaultValue;
    };

    auto stringToTagMode = [](const std::string& value) -> IWTagMode {
        if (value == "squawk") {
            return IWTagMode::Squawk;
        }
        return IWTagMode::Callsign;
    };

    auto readBoolWithDefault = [&jsonData, this](const std::string& key, bool defaultValue) -> bool {
        if (jsonData.contains("styling") && jsonData["styling"].contains(key)) {
            return jsonData["styling"][key].get<bool>();
        }
        return defaultValue;
    };

    return IWStyling{
        readColor("windowFrameColor"),
        readColor("windowFrameTextColor"),
        readColor("windowOuterFrameColor"),
        readColor("backgroundColor"),
        readColor("glideslopeColor"),
        readColor("localizerColor"),
        readColor("radarTargetColor"),
        readColor("historyTrailColor"),
        readColor("targetLabelColor"),
        readColor("rangeStatusTextColor"),
        readUIntWithDefault("fontSize", 12),
        readBoolWithDefault("showTagByDefault", true),
        stringToTagMode(readStringWithDefault("defaultTagMode", "callsign"))
    };
}

IWBehaviourSettings IWPlugin::ReadBehaviourSettings(const std::string& jsonFilePath) {
    std::ifstream file(jsonFilePath);
    if (!file.is_open()) {
        this->ShowErrorMessage("Could not load behaviour options", "Unable to open JSON file '" + jsonFilePath + "'");
    }

    nlohmann::json jsonData;
    file >> jsonData;

    nlohmann::json jsonObject = jsonData["behaviour"];

    return IWBehaviourSettings{
        jsonObject.at("openWindowsBasedOnActiveRunways").get<bool>()
    };
}

std::string IWPlugin::GetPluginDirectory() {
    char modulePath[MAX_PATH];
    GetModuleFileNameA((HINSTANCE)&__ImageBase, modulePath, sizeof(modulePath));
    std::string pluginDirectory = std::string(modulePath).substr(0, std::string(modulePath).find_last_of("\\/"));
    return pluginDirectory;
}

void IWPlugin::OnWindowClosed(IWWindow* window)
{
    auto it = std::find(windows.begin(), windows.end(), window);
    if (it != windows.end()) {
        windows.erase(it);
    }
}

void IWPlugin::OnWindowMenuOpenNew(std::string approachTitle)
{
    auto selectedApproach = std::find_if(availableApproaches.begin(), availableApproaches.end(),
        [&approachTitle](const IWApproachDefinition& approach) {
            return approach.title == approachTitle;
        });

    if (selectedApproach != availableApproaches.end()) {
        OpenNewWindow(&(*selectedApproach));
    }
    else {
        OpenNewWindow(&availableApproaches[0]);
    }
}

void IWPlugin::OnWindowRectangleChanged(IWWindow* window)
{
    CRect windowRect;
    window->GetWindowRect(&windowRect);
    std::string name = window->GetActiveApproachName();
    std::string description = name + " pos.";
    std::string rect = std::to_string(windowRect.left) + "," + std::to_string(windowRect.top) + "," + std::to_string(windowRect.right) + "," + std::to_string(windowRect.bottom);
    SaveDataToSettings(name.c_str(), description.c_str(), rect.c_str());
}

void IWPlugin::LoadSavedWindowPositions()
{
    // For every available approach, check if there is a saved position
    for (auto& approach : availableApproaches) {
        const char* settings = GetDataFromSettings(approach.title.c_str());
        if (settings) {
            std::string settingsString = std::string(settings);
            std::regex regex("([0-9]+),([0-9]+),([0-9]+),([0-9]+)");
            std::smatch match;
            if (std::regex_search(settingsString, match, regex)) {
                CRect rect;
                rect.left = std::stoi(match[1]);
                rect.top = std::stoi(match[2]);
                rect.right = std::stoi(match[3]);
                rect.bottom = std::stoi(match[4]);
                savedWindowPositions[approach.title] = rect;
            }
        }
    }
}


bool IWPlugin::OnCompileCommand(const char* sCommandLine)
{
    const std::string generalError = "Could not open ILS window";
    const std::string command(sCommandLine);
    const std::string prefix = ".ils ";

    if (command.rfind(prefix, 0) != 0) { // Command must start with ".ils "
        return false; // Not handled
    }

    // Extract the argument after ".ils "
    std::string argument = stringToUpper(trimString(command.substr(prefix.length())));

    if (argument.empty()) {
        this->ShowErrorMessage(generalError, "No approach name specified after '.ils'.");
        return false;
    }

    // Find the approach by title
    auto it = std::find_if(this->availableApproaches.begin(), this->availableApproaches.end(),
        [&argument](const IWApproachDefinition& approach) {
            return approach.title == argument;
        });

    if (it != this->availableApproaches.end()) {
        // Approach found: Open the approach window
        this->OpenNewWindow(&(*it));
        return true; // Command handled
    }
    else {
        // Approach not found
        this->ShowErrorMessage(generalError, "Approach '" + argument + "' not found.");
        return false; // Command not handled
    }
}

void IWPlugin::ShowErrorMessage(std::string consequence, std::string details)
{
    this->DisplayUserMessage(MY_PLUGIN_NAME, consequence.c_str(), details.c_str(), false, true, false, true, false);
}