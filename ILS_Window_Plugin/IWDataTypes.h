#pragma once

#include <string>
#include <vector>
#include <memory>

class IWWindow;

struct RGB {
    int r, g, b;
};

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
    RGB windowFrameColor;
    RGB windowFrameTextColor;
    RGB windowOuterFrameColor;
    RGB backgroundColor;
    RGB glideslopeColor;
    RGB localizerColor;
    RGB radarTargetColor;
    RGB historyTrailColor;
    RGB targetLabelColor;
    RGB rangeStatusTextColor;
    unsigned int fontSize;
    bool showTagByDefault;
    IWTagMode defaultTagMode;
};

struct IWBehaviourSettings {
    bool openWindowsBasedOnActiveRunways;
};