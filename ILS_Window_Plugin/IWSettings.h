#pragma once

#include "IWDataTypes.h"
#include "EuroScopePlugIn.h"
#include <string>
#include <map>
#include <vector>

#define MY_PLUGIN_NAME          "ILS Window Plugin"
#define MY_PLUGIN_VERSION       PLUGIN_VERSION
#define MY_PLUGIN_DEVELOPER     CONTRIBUTORS
#define MY_PLUGIN_COPYRIGHT     "GPL v3"

#define CONFIG_FILE_NAME        "ILS_window_plugin-config.json"

class IWSettings
{
public:
    IWSettings(EuroScopePlugIn::CPlugIn* plugin);

    void StoreWindowPositon(const std::string& approachName, CRect windowRect);
    CRect* GetWindowPositon(const std::string& approachName);
    std::vector<IWApproachDefinition> GetAvailableApproaches();
    IWConfig GetConfig();


private:
    EuroScopePlugIn::CPlugIn* euroscopePluginRef;
    std::vector<IWApproachDefinition> availableApproaches;
    std::map<std::string, CRect> cachedWindowPositions;

    std::vector<IWApproachDefinition> ReadApproachDefinitions(const std::string& jsonFilePath);
    IWBehaviourSettings ReadBehaviourSettings(const std::string& jsonFilePath);
    IWStyling ReadStyling(const std::string& jsonFilePath);
    void LoadWindowPositionSettings();

    void ShowErrorMessage(std::string consequence, std::string details);

    std::string GetPluginDirectory();

    IWStyling windowStyling;
    IWBehaviourSettings behaviourSettings;
};
