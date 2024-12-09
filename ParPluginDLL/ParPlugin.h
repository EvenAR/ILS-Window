#pragma once

#include <EuroScopePlugIn.h>
#include "ParWindow.h"
#include "ParDataTypes.h"
#include <vector>

class ParPlugin : public EuroScopePlugIn::CPlugIn, IParWindowEventListener {
private:
    std::vector<ParWindow*> windows;
    std::vector<ParApproachDefinition> availableApproaches;
    ParStyling windowStyling;
    ParBehaviourSettings behaviourSettings;

    void OpenNewWindow(ParApproachDefinition* approach);
    void SyncWithActiveRunways();

    bool autoOpenWhenRunwaysChanges = true;

    CPoint nextWindowSpawnPointM{ 50, 50 };

    // Euroscope API
    bool OnCompileCommand(const char* sCommandLine) override;
    void OnTimer(int seconds) override;
    void OnAirportRunwayActivityChanged() override;

public:
    ParPlugin();
    ~ParPlugin();
    void OnWindowClosed(ParWindow* window) override;

    std::vector<ParApproachDefinition> ReadApproachDefinitions(const std::string& jsonFilePath);
    ParStyling ReadStyling(const std::string& iniFilePath);
    ParBehaviourSettings ReadBehaviourSettings(const std::string& jsonFilePath);
    std::string GetPluginDirectory();
};
