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

struct ParStyling {
    RGB windowFrameColor;
    RGB windowFrameTextColor;
    RGB backgroundColor;
    RGB glideslopeColor;
    RGB localizerColor;
    RGB radarTargetColor;
    RGB historyTrailColor;
    RGB targetLabelColor;
    RGB rangeStatusTextColor;
};

struct ParBehaviourSettings {
    bool openWindowsBasedOnActiveRunways;
};