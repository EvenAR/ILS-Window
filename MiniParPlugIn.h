#include "lib/EuroScopePlugIn.h"
#include "ParWindow.h"
#include "ParDataTypes.h"
#include <vector>

#pragma once
class MiniParPlugIn : public EuroScopePlugIn::CPlugIn, IParWindowEventListener {
private:
    std::vector<ParWindow*> windows;
    std::vector<ParApproachDefinition> availableApproaches;
    ParStyling windowStyling;

    void OpenNewWindow(ParApproachDefinition* approach);
    void RemoveWindowFromList(ParWindow* window);

public:
    MiniParPlugIn();
    void OnWindowClosed(ParWindow* window) override;
    void OnTimer(int seconds) override;

    std::vector<ParApproachDefinition> ReadApproachDefinitions(const std::string& iniFilePath);
    ParStyling ReadStyling(const std::string& iniFilePath);
    std::string GetPluginDirectory();
};
