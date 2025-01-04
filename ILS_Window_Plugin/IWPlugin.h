#pragma once

#include <EuroScopePlugIn.h>
#include <vector>
#include "IWWindow.h"
#include "IWDataTypes.h"

#define WINDOW_CLASS_NAME _T("IWWindow")

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

    // Events from a window
    void OnWindowClosed(IWWindow* window) override;
    void OnNewWindowSelected() override;
public:
    IWPlugin();
    ~IWPlugin();

    std::vector<IWApproachDefinition> ReadApproachDefinitions(const std::string& jsonFilePath);
    IWStyling ReadStyling(const std::string& iniFilePath);
    IWBehaviourSettings ReadBehaviourSettings(const std::string& jsonFilePath);
    std::string GetPluginDirectory();
};
