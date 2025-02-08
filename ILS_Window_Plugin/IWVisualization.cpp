#include "pch.h"
#include "IWVisualization.h"
#include <cmath>

BEGIN_MESSAGE_MAP(IWVisualization, CWnd)
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_WM_MOVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_ERASEBKGND()
    ON_WM_MOUSEWHEEL()
    ON_WM_TIMER()
END_MESSAGE_MAP()

IWVisualization::IWVisualization(IWApproachDefinition selectedApproach, IWStyling styling, CFont* font)
{
    this->selectedApproach = selectedApproach;
    this->approachLength = selectedApproach.defaultRange;

    this->leftToRight = selectedApproach.localizerCourse > 0 && selectedApproach.localizerCourse < 180;
    this->showTagsByDefault = styling.showTagByDefault;
    this->applyTemperatureCorrection = true;
    this->tagMode = styling.defaultTagMode;

    this->rangeStatusTextColor = styling.rangeStatusTextColor;
    this->windowBackground = styling.backgroundColor;
    this->targetLabelColor = styling.targetLabelColor;
    this->glideSlopePen.CreatePen(PS_SOLID, 1, styling.glideslopeColor);
    this->localizerBrush.CreateSolidBrush(styling.localizerColor);
    this->radarTargetPen.CreatePen(PS_SOLID, 1, styling.radarTargetColor);
    this->historyTrailPen.CreatePen(PS_SOLID, 1, styling.historyTrailColor);

    this->font = font;
}


void IWVisualization::OnPaint()
{
    CPaintDC dc(this); // Device context for painting

    // Create a memory DC for double buffering
    CRect rect;
    GetClientRect(&rect);
    CDC memDC;
    memDC.CreateCompatibleDC(&dc);

    CBitmap bufferBitmap;
    bufferBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
    CBitmap* pOldBitmap = memDC.SelectObject(&bufferBitmap);

    // Clear the background
    memDC.FillSolidRect(rect, windowBackground);

    // Select font
    CFont* oldFont = memDC.SelectObject(font);

    // Perform all drawing operations on memDC instead of dc
    DrawGlideslopeAndLocalizer(memDC);
    DrawRadarTargets(memDC);
    if (showCurrentZoomValue) {
        DrawCurrentZoomValue(memDC);
    }

    // Restore the font
    memDC.SelectObject(oldFont);

    // Copy the buffer to the screen
    dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

    // Cleanup
    memDC.SelectObject(pOldBitmap);
}

void IWVisualization::DrawGlideslopeAndLocalizer(CDC& dc)
{
    // Draw glideslope line
    CPen* pOldPen = dc.SelectObject(&glideSlopePen);
    dc.MoveTo(glidePathTop);
    dc.LineTo(glidePathBottom);
    dc.SelectObject(pOldPen);

    // Draw localizer distance dots
    CBrush* pOldBrush = dc.SelectObject(&localizerBrush);
    for (int i = 0; i <= approachLength; i++)           // Draw distance points. Every 5th point is large
    {
        int radius = i % 5 == 0 ? 4 : 2;
        int x = glidePathBottom.x - i * pixelsPerNauticalMile;
        int y = glidePathBottom.y;

        DrawDiamond(CPoint(x, y), radius, dc);
    }
    dc.SelectObject(pOldBrush);
}

void IWVisualization::DrawRadarTargets(CDC& dc)
{
    if (m_latestLiveData == NULL) return;

    // Draw the radar targets
    for (const IWRadarTarget& radarTarget : m_latestLiveData->radarTargets)
    {
        auto it = radarTarget.positionHistory.rbegin();
        auto end = radarTarget.positionHistory.rend();

        // Draw the history trail in reverese so the newest dots appear on top
        for (auto it = radarTarget.positionHistory.rbegin(); it != radarTarget.positionHistory.rend(); ++it) {
            const IWTargetPosition& position = *it;
            bool isNewestPosition = (it + 1 == end);  // Check if the iterator is the last element

            CPoint ptTopView, ptSideView;
            if (!CalculateTargetCoordinates(position, ptTopView, ptSideView)) {
                // Target is outside the visible area
                continue;
            }

            int crossRadius = isNewestPosition ? TARGET_RADIUS : HISTORY_TRAIL_RADIUS;

            // Draw position seen from the side
            dc.SelectObject(isNewestPosition ? radarTargetPen : historyTrailPen);
            dc.MoveTo(CPoint(ptSideView.x, ptSideView.y - crossRadius));
            dc.LineTo(CPoint(ptSideView.x, ptSideView.y + crossRadius + 1));
            dc.MoveTo(CPoint(ptSideView.x - crossRadius, ptSideView.y));
            dc.LineTo(CPoint(ptSideView.x + crossRadius + 1, ptSideView.y));

            if (isNewestPosition) {
                dc.SelectStockObject(NULL_BRUSH);
                dc.Ellipse(ptSideView.x - crossRadius, ptSideView.y - crossRadius, ptSideView.x + crossRadius + 1, ptSideView.y + crossRadius + 1);
            }

            // Draw position seen from above
            dc.SelectObject(isNewestPosition ? radarTargetPen : historyTrailPen);
            dc.MoveTo(CPoint(ptTopView.x, ptTopView.y - crossRadius));
            dc.LineTo(CPoint(ptTopView.x, ptTopView.y + crossRadius + 1));
            dc.MoveTo(CPoint(ptTopView.x - crossRadius, ptTopView.y));
            dc.LineTo(CPoint(ptTopView.x + crossRadius + 1, ptTopView.y));

            // Draw the main target
            if (isNewestPosition) {
                dc.SelectStockObject(NULL_BRUSH);
                dc.Ellipse(ptTopView.x - crossRadius, ptTopView.y - crossRadius, ptTopView.x + crossRadius + 1, ptTopView.y + crossRadius + 1);

                // Check if callsign is in the vector of clicked targets
                bool isClicked = this->clickedTargets.find(radarTarget.callsign) != this->clickedTargets.end();

                if (this->showTagsByDefault) {
                    if (isClicked) continue;
                }
                else {
                    if (!isClicked) continue;
                }

                dc.MoveTo(ptTopView);
                dc.LineTo(ptTopView.x + LABEL_OFFSET, ptTopView.y + LABEL_OFFSET);

                // Callsign label
                dc.SetTextColor(targetLabelColor);
                dc.SetBkMode(TRANSPARENT);


                CString targetLabel;
                if (this->tagMode == IWTagMode::Squawk) {
                    targetLabel.Format(_T("%s"), radarTarget.squawk.c_str());
                }
                else if (this->tagMode == IWTagMode::Callsign) {
                    targetLabel.Format(_T("%s"), radarTarget.callsign.c_str());
                }

                // Draw the label
                CSize textSize = dc.GetTextExtent(targetLabel);
                CPoint labelPosition(ptTopView.x + LABEL_OFFSET, ptTopView.y + LABEL_OFFSET);
                CRect labelRect(labelPosition.x, labelPosition.y, labelPosition.x + textSize.cx, labelPosition.y + textSize.cy * 2);
                dc.DrawText(targetLabel, labelRect, DT_LEFT);
            }
        }
    }
}

void IWVisualization::DrawCurrentZoomValue(CDC& dc)
{
    CRect rect;
    GetClientRect(&rect);

    dc.SetTextColor(this->rangeStatusTextColor);
    dc.SetBkMode(TRANSPARENT);

    CString zoomMessage;
    zoomMessage.Format(_T("%d NM"), this->approachLength);

    CSize textSize = dc.GetTextExtent(zoomMessage);
    CPoint position(
        this->leftToRight ? rect.right - textSize.cx - 5 : rect.left + 5,
        rect.top + 5
    );
    CRect textRect(position.x, position.y, position.x + textSize.cx, position.y + textSize.cy * 2);

    dc.DrawText(zoomMessage, textRect, this->leftToRight ? DT_RIGHT : DT_LEFT);
}

void IWVisualization::SetActiveApproach(IWApproachDefinition selectedApproach)
{
    this->selectedApproach = selectedApproach;
    this->approachLength = selectedApproach.defaultRange;
    this->leftToRight = selectedApproach.localizerCourse > 0 && selectedApproach.localizerCourse < 180;

    UpdateDimentions();
    Invalidate();
}

void IWVisualization::SetLatestLiveData(IWLiveData* liveData)
{
    this->m_latestLiveData = liveData;
    Invalidate();
}

void IWVisualization::DrawDiamond(CPoint pt, int radius, CDC& dc)
{
    CPoint pts[5];
    pts[0] = CPoint(pt.x, pt.y - radius);
    pts[1] = CPoint(pt.x + radius, pt.y);
    pts[2] = CPoint(pt.x, pt.y + radius);
    pts[3] = CPoint(pt.x - radius, pt.y);
    pts[4] = CPoint(pt.x, pt.y - radius);

    dc.SetPolyFillMode(ALTERNATE);
    dc.Polygon(pts, 5);
}

BOOL IWVisualization::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    if (zDelta > 0 && this->approachLength > 5)
    {
        this->approachLength -= 1;
    }
    else if (zDelta < 0 && this->approachLength < 50)
    {
        this->approachLength += 1;
    }

    UpdateDimentions();
    Invalidate();

    SetTimer(zoomMessageTimerId, 1000, nullptr);

    showCurrentZoomValue = true;

    return TRUE;
}

void IWVisualization::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    if (nType != SIZE_MINIMIZED)
    {
        UpdateDimentions();
        Invalidate();
    }
}

bool IWVisualization::CalculateTargetCoordinates(const IWTargetPosition& position, CPoint& ptTopView, CPoint& ptSideView)
{
    // Calculate position relative to the runway
    double distanceToThreshold = CalculateDistance(position.latitude, position.longitude, selectedApproach.thresholdLatitude, selectedApproach.thresholdLongitude);
    double directionToThreshold = CalculateBearing(position.latitude, position.longitude, selectedApproach.thresholdLatitude, selectedApproach.thresholdLongitude);
    double angleDiff = (selectedApproach.localizerCourse - directionToThreshold) / 180.0 * PI; // anglediff in radians
    double heightAboveThreshold = position.trueAltitude - selectedApproach.thresholdAltitude;

    auto airportTemperature = m_latestLiveData->airportTemperatures.find(selectedApproach.airport);
    if (applyTemperatureCorrection && airportTemperature != m_latestLiveData->airportTemperatures.end()) {
        // In newer flight simulators true altitude is affected by the temperature.
        // In cold weather aircraft will be shown higher than they actually are, unless we correct for it.
        // See: https://forums.flightsimulator.com/t/vatsim-ivao-pilotedge-users-be-aware-of-an-important-bug/426142/468
        int temperatureCorrection = CalculateTemperatureCorrection(position.trueAltitude, selectedApproach.thresholdAltitude, airportTemperature->second);
        heightAboveThreshold -= temperatureCorrection;
    }

    double projectedDistanceFromThreshold = distanceToThreshold * cos(angleDiff);
    double projectedDistanceFromExtendedCenterline = distanceToThreshold * tan(angleDiff);

    if (projectedDistanceFromExtendedCenterline < 0 && abs(projectedDistanceFromExtendedCenterline) > selectedApproach.maxOffsetLeft) {
        // Too far left
        return false;
    }
    if (projectedDistanceFromExtendedCenterline > 0 && abs(projectedDistanceFromExtendedCenterline) > selectedApproach.maxOffsetRight) {
        // Too far right
        return false;
    }
    if (projectedDistanceFromThreshold < 0) {
        // Wrong side of the threshold
        return false;
    }
    if (heightAboveThreshold > approachHeightFt) {
        // Too high
        return false;
    }

    int xPosition = glidePathBottom.x - projectedDistanceFromThreshold * pixelsPerNauticalMile;
    int yPositionSlope = glidePathBottom.y - heightAboveThreshold * pixelsPerFt;
    int yPositionCenterline = glidePathBottom.y + projectedDistanceFromExtendedCenterline * pixelsPerNauticalMile;
    ptSideView = CPoint(xPosition, yPositionSlope);
    ptTopView = CPoint(xPosition, yPositionCenterline);

    return true;
}

void IWVisualization::UpdateDimentions()
{
    CRect rect;
    GetClientRect(&rect);

    const int CALC_SIDE_MARGIN = rect.Width() * APP_LINE_MARGIN_SIDES;
    const int CALC_TOP_MARGIN = rect.Height() * APP_LINE_MARGIN_TOP;
    const int CALC_BOTTOM_MARGIN = rect.Height() * APP_LINE_MARGIN_BOTTOM;

    // Define the start and end coordintes for rendering the glidepath
    this->glidePathTop = CPoint(leftToRight ? rect.left + CALC_SIDE_MARGIN : rect.right - CALC_SIDE_MARGIN, rect.top + CALC_TOP_MARGIN);
    this->glidePathBottom = CPoint(leftToRight ? rect.right - CALC_SIDE_MARGIN : rect.left + CALC_SIDE_MARGIN, rect.bottom - CALC_BOTTOM_MARGIN);

    this->approachHeightFt = (approachLength * FT_PER_NM * sin(selectedApproach.glideslopeAngle / 180.0 * PI));

    this->pixelsPerFt = (glidePathBottom.y - glidePathTop.y) / approachHeightFt;
    this->pixelsPerNauticalMile = (glidePathBottom.x - glidePathTop.x) / float(approachLength); // PS: negative when direction is left->right
}

void IWVisualization::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == zoomMessageTimerId)
    {
        // Hide the zoom message
        showCurrentZoomValue = false;
        KillTimer(zoomMessageTimerId);

        // Invalidate to redraw without the message
        CRect updateRect;
        GetClientRect(&updateRect);
        InvalidateRect(updateRect);
    }

    CWnd::OnTimer(nIDEvent);
}

void IWVisualization::OnLButtonDown(UINT nFlags, CPoint point)
{
    for (const IWRadarTarget& radarTarget : m_latestLiveData->radarTargets) {
        auto newestPosition = radarTarget.positionHistory.size() > 0 ? &radarTarget.positionHistory.front() : nullptr;

        if (newestPosition) {
            const IWTargetPosition& position = *newestPosition;

            CPoint ptTopView, ptSideView;
            if (!CalculateTargetCoordinates(position, ptTopView, ptSideView)) {
                // Target is outside the visible area
                continue;
            }

            CRect targetRect(ptTopView.x - TARGET_RADIUS, ptTopView.y - TARGET_RADIUS, ptTopView.x + TARGET_RADIUS, ptTopView.y + TARGET_RADIUS);

            if (targetRect.PtInRect(point))
            {
                bool isAlreadyClicked = this->clickedTargets.find(radarTarget.callsign) != this->clickedTargets.end();

                if (isAlreadyClicked) {
                    this->clickedTargets.erase(radarTarget.callsign);
                }
                else {
                    this->clickedTargets.insert(radarTarget.callsign);
                }
                CRect updateRect;
                GetClientRect(&updateRect);
                InvalidateRect(updateRect);
                break;
            }
        }
    }

    CWnd::OnLButtonDown(nFlags, point);
}

double IWVisualization::CalculateDistance(double lat1, double lon1, double lat2, double lon2) {
    // Convert latitude and longitude from degrees to radians
    lat1 = lat1 * PI / 180.0;
    lon1 = lon1 * PI / 180.0;
    lat2 = lat2 * PI / 180.0;
    lon2 = lon2 * PI / 180.0;
    // Haversine formula
    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;
    double a = pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlon / 2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double distance = EARTH_RADIUS_NM * c;
    return distance;
}

double IWVisualization::CalculateBearing(double lat1, double lon1, double lat2, double lon2) {
    // Convert latitude and longitude from degrees to radians
    lat1 = lat1 * PI / 180.0;
    lon1 = lon1 * PI / 180.0;
    lat2 = lat2 * PI / 180.0;
    lon2 = lon2 * PI / 180.0;

    // Calculate the bearing
    double y = sin(lon2 - lon1) * cos(lat2);
    double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(lon2 - lon1);
    double bearing = atan2(y, x);
    bearing = fmod(bearing + 2 * PI, 2 * PI);

    // Convert to degrees
    return bearing * 180.0 / PI;
}

double IWVisualization::CalculateTemperatureCorrection(int planePressAlt, int airportPressureAlt, double surfTemp) {
    double isaDeviation = surfTemp - 15;

    // Formula is from here: https://www.pprune.org/tech-log/573002-accurate-temperature-correction-formula.html
    return ((-isaDeviation / -0.0019812) * std::log(1 + (-0.0019812 * planePressAlt) / (288.15 + -0.0019812 * airportPressureAlt)));
}

BOOL IWVisualization::OnEraseBkgnd(CDC* pDC)
{
    // Do nothing here to prevent background clearing
    return TRUE;
}

const bool IWVisualization::GetLeftToRight()
{
    return this->leftToRight;
}

const bool IWVisualization::GetShowTagsByDefault()
{
    return this->showTagsByDefault;
}

const bool IWVisualization::GetApplyTemperatureCorrection()
{
    return this->applyTemperatureCorrection;
}

void IWVisualization::SetLeftToRight(bool leftToRight)
{
    this->leftToRight = leftToRight;
    UpdateDimentions();
    Invalidate();
}

void IWVisualization::SetShowTagsByDefault(bool showTagsByDefault)
{
    this->showTagsByDefault = showTagsByDefault;
    Invalidate();
}

void IWVisualization::SetApplyTemperatureCorrection(bool applyTemperatureCorrection)
{
    this->applyTemperatureCorrection = applyTemperatureCorrection;
    Invalidate();
}