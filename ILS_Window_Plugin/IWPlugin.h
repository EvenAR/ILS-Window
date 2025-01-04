#pragma once

#include <EuroScopePlugIn.h>
#include <vector>
#include "IWWindow.h"
#include "IWDataTypes.h"
#include <map>

#define WINDOW_CLASS_NAME _T("IWWindow")

class IWPlugin : public EuroScopePlugIn::CPlugIn, IIWWndEventListener {
private:
    std::vector<IWWindow*> windows;
    std::vector<IWApproachDefinition> availableApproaches;
    IWStyling windowStyling;
    IWBehaviourSettings behaviourSettings;
    std::map<std::string, CRect> savedWindowPositions;

    void OpenNewWindow(IWApproachDefinition* approach);
    void SyncWithActiveRunways();
    void LoadSavedWindowPositions();

    bool autoOpenWhenRunwaysChanges = true;

    // Euroscope API
    bool OnCompileCommand(const char* sCommandLine) override;
    void OnTimer(int seconds) override;
    void OnAirportRunwayActivityChanged() override;

    // Events from a window
    void OnWindowClosed(IWWindow* window) override;
    void OnWindowMenuOpenNew() override;
public:
    IWPlugin();
    ~IWPlugin();

    std::vector<IWApproachDefinition> ReadApproachDefinitions(const std::string& jsonFilePath);
    IWStyling ReadStyling(const std::string& iniFilePath);
    IWBehaviourSettings ReadBehaviourSettings(const std::string& jsonFilePath);
    std::string GetPluginDirectory();
};
