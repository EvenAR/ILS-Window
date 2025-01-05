#pragma once

#include <EuroScopePlugIn.h>
#include <vector>
#include "IWWindow.h"
#include "IWDataTypes.h"
#include <map>

#define WINDOW_CLASS_NAME _T("IWWindow")

#define MY_PLUGIN_NAME          "ILS Window Plugin"
#define MY_PLUGIN_VERSION       PLUGIN_VERSION
#define MY_PLUGIN_DEVELOPER     CONTRIBUTORS
#define MY_PLUGIN_COPYRIGHT     "GPL v3"

#define CONFIG_FILE_NAME        "ILS_window_plugin-config.json"

class IWPlugin : public EuroScopePlugIn::CPlugIn, IIWWndEventListener {
private:
    std::vector<IWWindow*> windows;
    std::vector<IWApproachDefinition> availableApproaches;
    IWStyling windowStyling;
    IWBehaviourSettings behaviourSettings;
    std::map<std::string, CRect> savedWindowPositions;
    std::map<std::string, float> airportTemperatures;

    void OpenNewWindow(IWApproachDefinition* approach);
    void SyncWithActiveRunways();
    void LoadSavedWindowPositions();
    void ShowErrorMessage(std::string consequence, std::string details);

    bool autoOpenWhenRunwaysChanges = true;

    // Euroscope API
    bool OnCompileCommand(const char* sCommandLine) override;
    void OnTimer(int seconds) override;
    void OnAirportRunwayActivityChanged() override;
    void OnNewMetarReceived(const char* sStation, const char* sFullMetar) override;

    // Events from a window
    void OnWindowClosed(IWWindow* window) override;
    void OnWindowMenuOpenNew(std::string approachTitle) override;
    void OnWindowRectangleChanged(IWWindow* window) override;
public:
    IWPlugin();
    ~IWPlugin();

    std::vector<IWApproachDefinition> ReadApproachDefinitions(const std::string& jsonFilePath);
    IWStyling ReadStyling(const std::string& iniFilePath);
    IWBehaviourSettings ReadBehaviourSettings(const std::string& jsonFilePath);
    std::string GetPluginDirectory();
};
