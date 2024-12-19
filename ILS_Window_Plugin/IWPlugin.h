#pragma once

#include <EuroScopePlugIn.h>
#include <vector>
#include "IWWindow.h"
#include "IWDataTypes.h"

class IWPlugin : public EuroScopePlugIn::CPlugIn, IIWWndEventListener {
private:
    std::vector<IWWindow*> windows;
    std::vector<IWApproachDefinition> availableApproaches;
    IWStyling windowStyling;
    IWBehaviourSettings behaviourSettings;

    void OpenNewWindow(IWApproachDefinition* approach);
    void SyncWithActiveRunways();

    bool autoOpenWhenRunwaysChanges = true;

    CPoint nextWindowSpawnPointM{ 50, 50 };

    // Euroscope API
    bool OnCompileCommand(const char* sCommandLine) override;
    void OnTimer(int seconds) override;
    void OnAirportRunwayActivityChanged() override;

public:
    IWPlugin();
    ~IWPlugin();
    void OnWindowClosed(IWWindow* window) override;

    std::vector<IWApproachDefinition> ReadApproachDefinitions(const std::string& jsonFilePath);
    IWStyling ReadStyling(const std::string& iniFilePath);
    IWBehaviourSettings ReadBehaviourSettings(const std::string& jsonFilePath);
    std::string GetPluginDirectory();
};
