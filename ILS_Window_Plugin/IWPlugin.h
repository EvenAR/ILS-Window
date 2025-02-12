#pragma once

#include <EuroScopePlugIn.h>
#include <vector>
#include "IWDataTypes.h"
#include <map>
#include "IWSettings.h"
#include "IWWindowManager.h"

class IWPlugin : public EuroScopePlugIn::CPlugIn {
public:
    IWPlugin();
    ~IWPlugin();

private:
    IWWindowManager windowManager;
    std::map<std::string, int> airportTemperatures;
    IWSettings settings;

    // Euroscope callbacks
    bool OnCompileCommand(const char* sCommandLine) override;
    void OnTimer(int seconds) override;
    void OnAirportRunwayActivityChanged() override;
    void OnNewMetarReceived(const char* sStation, const char* sFullMetar) override;

    // Helper functions for the Euroscope API
    std::vector<IWActiveRunway> CollectActiveRunways(bool forArrival);
    void ShowErrorMessage(std::string consequence, std::string details);
};
