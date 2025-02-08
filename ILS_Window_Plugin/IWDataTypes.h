#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

class IWWindow;

struct IWTargetPosition {
    int trueAltitude;
    double latitude;
    double longitude;
};

struct IWRadarTarget {
    std::string callsign;
    std::string squawk;
    std::string icaoType;
    char wtcCategory;
    std::vector<IWTargetPosition> positionHistory;
};

struct IWLiveData {
    std::vector<IWRadarTarget> radarTargets;
    std::map<std::string, int> airportTemperatures;
};

struct IWApproachDefinition {
    std::string title;
    std::string airport;
    std::string runway;
    double thresholdLatitude;
    double thresholdLongitude;
    int thresholdAltitude;
    float localizerCourse;
    float glideslopeAngle;
    int defaultRange;
    float maxOffsetLeft;
    float maxOffsetRight;
};

enum IWTagMode {
    Squawk,
    Callsign
};

struct IWStyling {
    COLORREF windowFrameColor;
    COLORREF windowFrameTextColor;
    COLORREF windowOuterFrameColor;
    COLORREF backgroundColor;
    COLORREF glideslopeColor;
    COLORREF localizerColor;
    COLORREF radarTargetColor;
    COLORREF historyTrailColor;
    COLORREF targetLabelColor;
    COLORREF rangeStatusTextColor;
    unsigned int fontSize;
    bool showTagByDefault;
    IWTagMode defaultTagMode;
};

struct IWBehaviourSettings {
    bool openWindowsBasedOnActiveRunways;
    std::string windowStyle;
};