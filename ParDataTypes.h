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
    std::vector<ParTargetPosition> positionHistory;
};

struct ParData {
    std::vector<ParRadarTarget> radarTargets;
};

struct ParApproachDefinition {
    std::string title;
    double thresholdLatitude;
    double thresholdLongitude;
    int thresholdAltitude;
    float localizerCourse;
    float glideslopeAngle;
    int defaultRange;
    ParWindow* windowReference;
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
    RGB zoomStatusTextColor;
};