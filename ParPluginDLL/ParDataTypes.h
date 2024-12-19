#pragma once

#include <string>
#include <vector>
#include <memory>

class ParWindow;

struct RGB {
    int r, g, b;
};

struct ParTargetPosition {
    int heightAboveThreshold;
    double distanceToThreshold;
    double directionToThreshold;
};

struct ParRadarTarget {
    std::string callsign;
    std::string squawk;
    std::string icaoType;
    char wtcCategory;
    std::vector<ParTargetPosition> positionHistory;
};

struct ParData {
    std::vector<ParRadarTarget> radarTargets;
};

struct ParApproachDefinition {
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
    ParWindow* windowReference = nullptr;
};

enum ParTagMode {
    Squawk,
    Callsign
};

struct ParStyling {
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
    ParTagMode defaultTagMode;
};

struct ParBehaviourSettings {
    bool openWindowsBasedOnActiveRunways;
};