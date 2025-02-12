#pragma once
#include <afxwin.h>
#include "IWDataTypes.h"
#include <set>
#include <mutex>

#define APP_LINE_MARGIN_TOP 0.08
#define APP_LINE_MARGIN_SIDES 0.08
#define APP_LINE_MARGIN_BOTTOM 0.35
#define MAX_RANGE 30 // Nautical miles
#define LABEL_OFFSET 15
#define TARGET_RADIUS 5
#define HISTORY_TRAIL_RADIUS 5
#define FT_PER_NM 6076.11549
#define EARTH_RADIUS_NM 3440.065
#define PI 3.14159265359

class IWVisualization : public CWnd
{
public:
    IWVisualization(IWApproachDefinition selectedApproach, IWStyling styling, CFont* mainFont);
    void DrawGlideslopeAndLocalizer(CDC& dc);
    void DrawRadarTargets(CDC& dc);
    void DrawCurrentZoomValue(CDC& dc);
    void SetActiveApproach(IWApproachDefinition selectedApproach);
    void SetLatestLiveData(IWLiveData* liveData);

    // Settings gettters
    const bool GetLeftToRight();
    const bool GetShowTagsByDefault();
    const bool GetApplyTemperatureCorrection();

    // Settings setters
    void SetLeftToRight(bool leftToRight);
    void SetShowTagsByDefault(bool showTagsByDefault);
    void SetApplyTemperatureCorrection(bool applyTemperatureCorrection);

    DECLARE_MESSAGE_MAP()

private:
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

    void DrawDiamond(CPoint pt, int size, CDC& dc);
    bool CalculateTargetCoordinates(const IWTargetPosition& position, CPoint& ptTopView, CPoint& ptSideView);
    void UpdateDimentions();

    double CalculateDistance(double lat1, double lon1, double lat2, double lon2);
    double CalculateBearing(double lat1, double lon1, double lat2, double lon2);
    double CalculateTemperatureCorrection(int planePressAlt, int airportPressureAlt, double surfTemp);

    IWLiveData* m_latestLiveData = NULL;
    IWApproachDefinition selectedApproach;

    COLORREF rangeStatusTextColor;
    COLORREF windowBackground;
    COLORREF targetLabelColor;

    CPen glideSlopePen;
    CBrush localizerBrush;
    CPen radarTargetPen;
    CPen historyTrailPen;
    IWTagMode tagMode;
    CFont* mainFont;

    std::set<std::string> clickedTargets;

    int approachLength;
    bool showCurrentZoomValue = false;
    UINT_PTR zoomMessageTimerId = 1;

    // Dimentions
    float approachHeightFt;
    double pixelsPerFt;
    double pixelsPerNauticalMile;
    CPoint glidePathTop;
    CPoint glidePathBottom;

    // Settings
    bool showTagsByDefault;
    bool applyTemperatureCorrection;
    bool leftToRight;
};

