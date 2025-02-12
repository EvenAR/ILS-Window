#include "pch.h"
#include "IWSettings.h"
#include <regex>
#include <fstream>
#include <json.hpp>
#include "IWUtils.h"

using json = nlohmann::json;

IWSettings::IWSettings(EuroScopePlugIn::CPlugIn* plugin)
{
    this->euroscopePluginRef = plugin;

    auto configFilePath = this->GetPluginDirectory() + "\\" + CONFIG_FILE_NAME;
    this->availableApproaches = this->ReadApproachDefinitions(configFilePath);
    this->windowStyling = this->ReadStyling(configFilePath);
    this->behaviourSettings = this->ReadBehaviourSettings(configFilePath);

    this->LoadWindowPositionSettings();
}

void IWSettings::StoreWindowPositon(const std::string& approachName, CRect windowRect)
{
    std::string description = approachName + " pos.";
    std::string rect = std::to_string(windowRect.left) + "," + std::to_string(windowRect.top) + "," + std::to_string(windowRect.right) + "," + std::to_string(windowRect.bottom);


    this->euroscopePluginRef->SaveDataToSettings(approachName.c_str(), description.c_str(), rect.c_str());
    cachedWindowPositions[approachName] = windowRect;
}

CRect* IWSettings::GetWindowPositon(const std::string& approachName)
{
    // Check if the position is cached
    if (cachedWindowPositions.find(approachName) == cachedWindowPositions.end()) {
        return NULL;
    }
    else {
        return &cachedWindowPositions[approachName];
    }
}

std::vector<IWApproachDefinition> IWSettings::GetAvailableApproaches()
{
    return this->availableApproaches;
}

IWConfig IWSettings::GetConfig()
{
    return IWConfig{
        this->windowStyling,
        this->behaviourSettings
    };
}

void IWSettings::LoadWindowPositionSettings()
{
    // For every available approach, check if there is a saved position
    for (auto& approach : availableApproaches) {
        const char* settings = this->euroscopePluginRef->GetDataFromSettings(approach.title.c_str());
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
                cachedWindowPositions[approach.title] = rect;
            }
        }
    }
}

std::vector<IWApproachDefinition> IWSettings::ReadApproachDefinitions(const std::string& jsonFilePath) {
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

IWStyling IWSettings::ReadStyling(const std::string& jsonFilePath) {
    const std::string generalErrorMessage = "Could not load style settings";

    std::ifstream file(jsonFilePath);
    if (!file.is_open()) {
        this->ShowErrorMessage(generalErrorMessage, "Unable to open JSON file: " + jsonFilePath);
    }

    nlohmann::json jsonData;
    file >> jsonData;

    auto readColor = [&jsonData, &generalErrorMessage, this](const std::string& key) -> COLORREF {
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

IWBehaviourSettings IWSettings::ReadBehaviourSettings(const std::string& jsonFilePath) {
    std::ifstream file(jsonFilePath);
    if (!file.is_open()) {
        this->ShowErrorMessage("Could not load behaviour options", "Unable to open JSON file '" + jsonFilePath + "'");
    }

    nlohmann::json jsonData;
    file >> jsonData;

    nlohmann::json jsonObject = jsonData["behaviour"];

    auto readStringWithDefault = [&jsonData, this](const std::string& key, const std::string& defaultValue) -> std::string {
        if (jsonData.contains("behaviour") && jsonData["behaviour"].contains(key)) {
            return jsonData["behaviour"][key].get<std::string>();
        }
        return defaultValue;
        };

    return IWBehaviourSettings{
        jsonObject.at("openWindowsBasedOnActiveRunways").get<bool>(),
        readStringWithDefault("windowStyle", "X11")
    };
}

void IWSettings::ShowErrorMessage(std::string consequence, std::string details)
{
    this->euroscopePluginRef->DisplayUserMessage(MY_PLUGIN_NAME, consequence.c_str(), details.c_str(), false, true, false, true, false);
}

std::string IWSettings::GetPluginDirectory() {
    char modulePath[MAX_PATH];
    GetModuleFileNameA((HINSTANCE)&__ImageBase, modulePath, sizeof(modulePath));
    std::string pluginDirectory = std::string(modulePath).substr(0, std::string(modulePath).find_last_of("\\/"));
    return pluginDirectory;
}