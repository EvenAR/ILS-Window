#include "pch.h"
#include "IWPlugin.h"
#include <fstream>
#include "IWUtils.h"
#include <regex>
#include "IWCdeWindow.h"
#include "IWX11Window.h"

IWPlugin::IWPlugin(void) : 
    CPlugIn(EuroScopePlugIn::COMPATIBILITY_CODE, MY_PLUGIN_NAME, MY_PLUGIN_VERSION, MY_PLUGIN_DEVELOPER, MY_PLUGIN_COPYRIGHT),
    settings(this),
    windowManager(&settings)
{
    this->windowManager.SyncWithActiveRunways(this->CollectActiveRunways(true));
}

IWPlugin::~IWPlugin()
{
    
}

void IWPlugin::OnTimer(int seconds)
{
    IWLiveData liveData;

    for (auto rt = this->RadarTargetSelectFirst(); rt.IsValid(); rt = this->RadarTargetSelectNext(rt)) {
        std::vector<IWTargetPosition> positionHistory;

        EuroScopePlugIn::CRadarTargetPositionData previousPosition;

        for (int i = 0; i <= 9; ++i) { // Last 10 positions (for history trail)

            EuroScopePlugIn::CRadarTargetPositionData positionData =
                (i == 0) ? rt.GetPosition() : rt.GetPreviousPosition(previousPosition);

            const bool notOfInterest =
                positionData.GetReportedGS() < 30 || 
                positionData.GetPressureAltitude() > 10000;

            if (notOfInterest || !positionData.IsValid()) {
                break;
            }

            positionHistory.push_back({
                positionData.GetPressureAltitude(),
                positionData.GetPosition().m_Latitude,
                positionData.GetPosition().m_Longitude,
            });

            previousPosition = positionData;
        }

        auto correlatedFlightPlan = rt.GetCorrelatedFlightPlan();

        std::string aircraftIcaoType = correlatedFlightPlan.IsValid()
            ? std::string(correlatedFlightPlan.GetFlightPlanData().GetAircraftFPType())
            : std::string("");

        char aircraftWtc = correlatedFlightPlan.IsValid()
            ? correlatedFlightPlan.GetFlightPlanData().GetAircraftWtc()
            : ' ';

        if (positionHistory.size() > 0) {
            liveData.radarTargets.push_back({
                rt.GetCallsign(),
                rt.GetPosition().GetSquawk(),
                aircraftIcaoType,
                aircraftWtc,
                positionHistory
            });
            liveData.airportTemperatures = airportTemperatures;
        }
    }

    windowManager.HandleLiveData(liveData);
}

void IWPlugin::OnAirportRunwayActivityChanged()
{
    this->windowManager.SyncWithActiveRunways(CollectActiveRunways(true));
}

void IWPlugin::OnNewMetarReceived(const char* sStation, const char* sFullMetar)
{
    // Extract temperature from METAR to be used for altitude correction
    std::string station = std::string(sStation);
    std::string fullMetar = std::string(sFullMetar);
    std::regex temperatureRegex(R"((M?\d{2})\/(M?\d{2}))");
    std::smatch temperatureMatch;
    if (std::regex_search(fullMetar, temperatureMatch, temperatureRegex)) {
        std::string temperatureString = temperatureMatch[1];
        bool isNegative = temperatureString[0] == 'M';
        int temperature = std::stoi(temperatureString.substr(1));
        if (isNegative) {
            temperature = -temperature;
        }
        airportTemperatures[station] = temperature;
    }
}

std::vector<IWActiveRunway> IWPlugin::CollectActiveRunways(bool forArrival)
{
    std::vector<IWActiveRunway> activeRunways;

    for (auto airport = this->SectorFileElementSelectFirst(EuroScopePlugIn::SECTOR_ELEMENT_AIRPORT); airport.IsValid(); airport = this->SectorFileElementSelectNext(airport, EuroScopePlugIn::SECTOR_ELEMENT_AIRPORT)) {
        if (airport.IsElementActive(!forArrival)) {
            auto activeAirportIcao = std::string(airport.GetName());
            for (auto runway = this->SectorFileElementSelectFirst(EuroScopePlugIn::SECTOR_ELEMENT_RUNWAY); runway.IsValid(); runway = this->SectorFileElementSelectNext(runway, EuroScopePlugIn::SECTOR_ELEMENT_RUNWAY)) {
                auto runwayAirportName = trimString(std::string(runway.GetAirportName()));
                if (runwayAirportName == activeAirportIcao) {
                    for (int runwayDirection = 0; runwayDirection < 2; runwayDirection++) {
                        if (runway.IsElementActive(!forArrival, runwayDirection) && runwayAirportName == activeAirportIcao) {
                            auto runwayName = trimString(std::string(runway.GetRunwayName(runwayDirection)));
                            
                            activeRunways.push_back({
                                runwayAirportName,
                                runwayName
                            });
                        }
                    }
                }
            }
        }
    }

    return activeRunways;
}

bool IWPlugin::OnCompileCommand(const char* sCommandLine)
{
    const std::string generalError = "Could not open ILS window";
    const std::string command(sCommandLine);
    const std::string prefix = ".ils ";

    if (command.rfind(prefix, 0) != 0) { // Command must start with ".ils "
        return false; // Not handled
    }

    // Extract the argument after ".ils "
    std::string approachName = stringToUpper(trimString(command.substr(prefix.length())));
    if (approachName.empty()) {
        this->ShowErrorMessage(generalError, "No approach name specified after '.ils'.");
        return false;
    }

    bool success = this->windowManager.Open(approachName);

    if (!success) {
        this->ShowErrorMessage(generalError, "Could not find approach with name '" + approachName + "'.");
        return false;
    }
    else {
        return true; // Command was handled
    }
}

void IWPlugin::ShowErrorMessage(std::string consequence, std::string details)
{
    this->DisplayUserMessage(MY_PLUGIN_NAME, consequence.c_str(), details.c_str(), false, true, false, true, false);
}