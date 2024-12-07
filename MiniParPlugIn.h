#include "lib/EuroScopePlugIn.h"
#include "ParWindow.h"
#include "ParDataTypes.h"

#pragma once
class MiniParPlugIn : public EuroScopePlugIn::CPlugIn, IParWindowEventListener {
private:
    std::vector<ParWindow*> windows;
    ParStyling windowStyling;

    void OpenNewWindow(double appSlope, double appLength, bool leftToRight);
    void RemoveWindowFromList(ParWindow* window);

public:
    MiniParPlugIn();
    void OnWindowClosed(ParWindow* window) override;
    void OnTimer(int seconds) override;

    std::vector<ParApproachDefinition> ReadApproachDefinitions(const std::string& iniFilePath);
    ParStyling ReadStyling(const std::string& iniFilePath);
};
