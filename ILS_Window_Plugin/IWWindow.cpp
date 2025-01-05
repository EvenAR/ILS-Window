#include "pch.h"
#include "IWWindow.h"
#include <cmath>

#define MAX_PROCEDURES 100

#define MENU_ITEM_FLIP                      10000
#define MENU_ITEM_SHOW_LABELS               10001
#define MENU_ITEM_CORRECT_FOR_TEMPERATURE   10002
#define MENU_ITEM_PROCEDURES_SEL_START      20000
#define MENU_ITEM_PROCEDURES_NEW_START      30000

BEGIN_MESSAGE_MAP(IWWindow, CWnd)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_MOVE()
    ON_WM_SIZING()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_CTLCOLOR()
    ON_MESSAGE(WM_UPDATE_DATA, &IWWindow::OnUpdateData)
    ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
    ON_WM_NCACTIVATE()
    ON_WM_GETMINMAXINFO()
    ON_WM_MOUSEWHEEL()
    ON_WM_TIMER()
    ON_MESSAGE(WM_EXITSIZEMOVE, &IWWindow::OnExitSizeMove)
    ON_COMMAND_EX(MENU_ITEM_FLIP, &IWWindow::OnMenuOptionSelected)
    ON_COMMAND_EX(MENU_ITEM_SHOW_LABELS, &IWWindow::OnMenuOptionSelected)
    ON_COMMAND_EX(MENU_ITEM_CORRECT_FOR_TEMPERATURE, &IWWindow::OnMenuOptionSelected)
    ON_COMMAND_RANGE(MENU_ITEM_PROCEDURES_SEL_START, MENU_ITEM_PROCEDURES_SEL_START + MAX_PROCEDURES, &IWWindow::OnProcedureSelected)
    ON_COMMAND_RANGE(MENU_ITEM_PROCEDURES_NEW_START, MENU_ITEM_PROCEDURES_NEW_START + MAX_PROCEDURES, &IWWindow::OnProcedureSelected)
END_MESSAGE_MAP()

IWWindow::IWWindow(IWApproachDefinition selectedApproach, IWStyling styling) : titleBar(
    selectedApproach.title,
    RGB(styling.windowFrameColor.r, styling.windowFrameColor.g, styling.windowFrameColor.b),
    RGB(styling.windowFrameTextColor.r, styling.windowFrameTextColor.g, styling.windowFrameTextColor.b),
    RGB(styling.windowOuterFrameColor.r, styling.windowOuterFrameColor.g, styling.windowOuterFrameColor.b),
    this
)
{
    this->selectedApproach = selectedApproach;
    this->approachLength = selectedApproach.defaultRange;
    this->leftToRight = selectedApproach.localizerCourse > 0 && selectedApproach.localizerCourse < 180;

    this->rangeStatusTextColor = RGB(styling.rangeStatusTextColor.r, styling.rangeStatusTextColor.g, styling.rangeStatusTextColor.b);
    this->windowBackground = RGB(styling.backgroundColor.r, styling.backgroundColor.g, styling.backgroundColor.b);
    this->targetLabelColor = RGB(styling.targetLabelColor.r, styling.targetLabelColor.g, styling.targetLabelColor.b);
    this->windowBorderPen.CreatePen(PS_SOLID, 6, RGB(styling.windowFrameColor.r, styling.windowFrameColor.g, styling.windowFrameColor.b));
    this->windowOuterBorderPen.CreatePen(PS_SOLID, 1, RGB(styling.windowOuterFrameColor.r, styling.windowOuterFrameColor.g, styling.windowOuterFrameColor.b));
    this->glideSlopePen.CreatePen(PS_SOLID, 1, RGB(styling.glideslopeColor.r, styling.glideslopeColor.g, styling.glideslopeColor.b));
    this->localizerBrush.CreateSolidBrush(RGB(styling.localizerColor.r, styling.localizerColor.g, styling.localizerColor.b));
    this->radarTargetPen.CreatePen(PS_SOLID, 1, RGB(styling.radarTargetColor.r, styling.radarTargetColor.g, styling.radarTargetColor.b));
    this->historyTrailPen.CreatePen(PS_SOLID, 1, RGB(styling.historyTrailColor.r, styling.historyTrailColor.g, styling.historyTrailColor.b));

    float fontPointsSize = styling.fontSize * 72 / 96;
    this->euroScopeFont.CreatePointFont(int(fontPointsSize * 10), _T("EuroScope"));
    this->tagMode = styling.defaultTagMode;
    this->showTagsByDefault = styling.showTagByDefault;
}

IWWindow::~IWWindow()
{
    this->titleBar.DestroyWindow();
}

int IWWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect barRect(0, 0, lpCreateStruct->cx, TITLE_BAR_HEIGHT);
    if (!titleBar.CreateTopBar(this, barRect, IDC_TOPBAR))
    {
        AfxMessageBox(_T("Failed to create top bar"));
        return -1;  // Handle error appropriately
    }

    return 0;
}


void IWWindow::OnPaint()
{
    CPaintDC dc(this); // Device context for painting

    // Create a memory DC for double buffering
    CRect rect;
    GetClientRect(&rect);
    CBitmap bufferBitmap;
    CDC memDC;

    memDC.CreateCompatibleDC(&dc);
    bufferBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
    CBitmap* pOldBitmap = memDC.SelectObject(&bufferBitmap);

    // Clear the background
    memDC.FillSolidRect(rect, windowBackground);

    // Perform all drawing operations on memDC instead of dc
    DrawContent(memDC);

    // Copy the buffer to the screen
    dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

    // Cleanup
    memDC.SelectObject(pOldBitmap);
}

void IWWindow::DrawContent(CDC& dc)
{
    CRect rect = GetClientRectBelowTitleBar();
    CFont* oldFont = dc.SelectObject(&euroScopeFont);

    dc.FillSolidRect(rect, windowBackground);

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

    // Draw the radar targets
    for (const IWRadarTarget& radarTarget : m_latestLiveData.radarTargets)
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

    if (showZoomMessage)
    {
        CRect rect = GetClientRectBelowTitleBar();

        dc.SetTextColor(this->rangeStatusTextColor);
        dc.SetBkMode(TRANSPARENT);

        CString zoomMessage;
        zoomMessage.Format(_T("%d NM"), this->approachLength);

        CSize textSize = dc.GetTextExtent(zoomMessage);
        CPoint position(
            this->leftToRight ? rect.right - textSize.cx - 5 : rect.left + 5,
            rect.top + 5
        );
        CRect textRect(position.x, position.y, position.x + textSize.cx, position.y + textSize.cy*2);

        dc.DrawText(zoomMessage, textRect, this->leftToRight ? DT_RIGHT : DT_LEFT);
    }

    // Draw custom window border
    dc.SelectStockObject(NULL_BRUSH);
    dc.SelectObject(windowBorderPen);
    dc.Rectangle(rect);

    // Draw outer window border
    CRect fullWindowArea;
    GetClientRect(&fullWindowArea);
    dc.SelectObject(windowOuterBorderPen);
    dc.Rectangle(fullWindowArea);

    // Cleanup
    dc.SelectObject(oldFont);
}

void IWWindow::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    // Recalculate gradient or redraw on resize
    if (nType != SIZE_MINIMIZED)  // Ignore if window is minimized
    {
        UpdateDimentions();
        Invalidate(); // Mark the entire client area for repaint
    }

    if (titleBar.GetSafeHwnd())
    {
        CRect barRect(0, 0, cx, TITLE_BAR_HEIGHT);
        titleBar.MoveWindow(barRect);
    }
}

LRESULT IWWindow::OnExitSizeMove(WPARAM wParam, LPARAM lParam)
{
    // Notify listener about the end of the resize or move operation
    if (m_listener)
    {
        m_listener->OnWindowRectangleChanged(this);
    }
    return 0;
}

void IWWindow::OnSizing(UINT nSide, LPRECT lpRect)
{
    int width = lpRect->right - lpRect->left;
    int height = lpRect->bottom - lpRect->top;

    // Snap width and height to X px increments
    int snappedWidth = static_cast<int>(std::round(width / SIZE_SNAP_INCREMENTS) * SIZE_SNAP_INCREMENTS);
    int snappedHeight = static_cast<int>(std::round(height / SIZE_SNAP_INCREMENTS) * SIZE_SNAP_INCREMENTS);

    // Adjust the RECT based on the resizing side. 
    // It's always the top right corner in our case
    if (nSide == WMSZ_TOPRIGHT) {
        lpRect->top = lpRect->bottom - snappedHeight;
        lpRect->right = lpRect->left + snappedWidth;
    }

    CWnd::OnSizing(nSide, lpRect); // Call the base class handler
}

BOOL IWWindow::OnNcActivate(BOOL bActive)
{
    // Prevent Windows from redrawing the NC area
    return TRUE;
}

BOOL IWWindow::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CWnd::PreCreateWindow(cs))
        return FALSE;

    return TRUE;
}

BOOL IWWindow::OnEraseBkgnd(CDC* pDC)
{
    // Do nothing here to prevent background clearing
    return TRUE;
}

LRESULT IWWindow::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
    IWLiveData* pData = reinterpret_cast<IWLiveData*>(wParam);
    if (pData) {
        m_latestLiveData = *pData;
    }

    // Trigger a repaint
    CRect updateRect = GetClientRectBelowTitleBar(); // Adjust based on what needs redrawing
    InvalidateRect(&updateRect, FALSE); // FALSE to avoid erasing the background unnecessarily

    return 0;
}


CRect IWWindow::GetClientRectBelowTitleBar()
{
    CRect rect;
    GetClientRect(&rect);  // Get full client area

    // Adjust rect to exclude the top bar area (assume the top bar is 30 pixels high)
    CRect topBarRect;
    titleBar.GetWindowRect(&topBarRect);
    ScreenToClient(&topBarRect); // Convert to client coordinates

    rect.top = topBarRect.bottom; // Move the top to below the top bar

    return rect;
}

void IWWindow::DrawDiamond(CPoint pt, int radius, CDC& dc)
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

void IWWindow::OnResizeStart()
{
    SendMessage(WM_NCLBUTTONDOWN, HTTOPRIGHT, NULL); // Resize using the top right corner
}

void IWWindow::OnCloseButtonClicked()
{
    this->DestroyWindow();
}

void IWWindow::OnMenuButtonClicked()
{
    CPoint point;
    GetCursorPos(&point);
    CreatePopupMenu(point);
}

void IWWindow::OnDestroy()
{
    if (m_listener) {
        m_listener->OnWindowClosed(this);
    }
    // Call base class cleanup (this ensures MFC cleans up the window itself)
    CWnd::OnDestroy();
}

void IWWindow::SetListener(IIWWndEventListener* listener)
{
    m_listener = listener;
}

void IWWindow::SetAvailableApproaches(const std::vector<IWApproachDefinition>& approaches)
{
    this->availableApproaches = approaches;
}

void IWWindow::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize.x = 150; // Minimum width in pixels
    lpMMI->ptMinTrackSize.y = 100; // Minimum height in pixels
}

BOOL IWWindow::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
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

    CRect updateRect = GetClientRectBelowTitleBar();
    InvalidateRect(updateRect);

    SetTimer(zoomMessageTimerId, 1000, nullptr);

    showZoomMessage = true;

    return TRUE;
}

void IWWindow::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == zoomMessageTimerId)
    {
        // Hide the zoom message
        showZoomMessage = false;
        KillTimer(zoomMessageTimerId);

        // Invalidate to redraw without the message
        CRect updateRect = GetClientRectBelowTitleBar();
        InvalidateRect(updateRect);
    }

    CWnd::OnTimer(nIDEvent);
}

void IWWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
    for (const IWRadarTarget& radarTarget : m_latestLiveData.radarTargets) {
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
                CRect updateRect = GetClientRectBelowTitleBar();
                InvalidateRect(updateRect);
                break;
            }
        }
    }

    CWnd::OnLButtonDown(nFlags, point);
}

bool IWWindow::CalculateTargetCoordinates(const IWTargetPosition& position, CPoint& ptTopView, CPoint& ptSideView)
{
    // Calculate position relative to the runway
    double distanceToThreshold = CalculateDistance(position.latitude, position.longitude, selectedApproach.thresholdLatitude, selectedApproach.thresholdLongitude);
    double directionToThreshold = CalculateBearing(position.latitude, position.longitude, selectedApproach.thresholdLatitude, selectedApproach.thresholdLongitude);
    double angleDiff = (selectedApproach.localizerCourse - directionToThreshold) / 180.0 * PI; // anglediff in radians
    double heightAboveThreshold = position.pressureCorrectedAltitude - selectedApproach.thresholdAltitude;

    if (applyTemperatureCorrection) {
        // In newer flight simulators true altitude is affected by the temperature.
        // In cold weather aircraft will be shown higher than they actually are, unless we correct for it.
        // See: https://forums.flightsimulator.com/t/vatsim-ivao-pilotedge-users-be-aware-of-an-important-bug/426142/468
        int temperatureCorrection = CalculateTemperatureCorrection(position.pressureCorrectedAltitude, selectedApproach.thresholdAltitude, m_latestLiveData.airportTemperatures[selectedApproach.airport]);
        heightAboveThreshold -= temperatureCorrection;
    }

    double projectedDistanceFromThreshold = distanceToThreshold * cos(angleDiff);
    double projectedDistanceFromExtendedCenterline = distanceToThreshold * tan(angleDiff);

    if (projectedDistanceFromExtendedCenterline < 0 && abs(projectedDistanceFromExtendedCenterline) > this->selectedApproach.maxOffsetLeft) {
        // Too far left
        return false;
    }
    if (projectedDistanceFromExtendedCenterline > 0 && abs(projectedDistanceFromExtendedCenterline) > this->selectedApproach.maxOffsetRight) {
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

void IWWindow::UpdateDimentions()
{
    CRect rect = GetClientRectBelowTitleBar();

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

double IWWindow::CalculateDistance(double lat1, double lon1, double lat2, double lon2) {
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

double IWWindow::CalculateBearing(double lat1, double lon1, double lat2, double lon2) {
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

double IWWindow::CalculateTemperatureCorrection(int planePressAlt, int airportPressureAlt, double surfTemp) {
    double isaDeviation = surfTemp - 15;

    // Formula is from here: https://www.pprune.org/tech-log/573002-accurate-temperature-correction-formula.html
    return ((-isaDeviation / -0.0019812) * std::log(1 + (-0.0019812 * planePressAlt) / (288.15 + -0.0019812 * airportPressureAlt)));
}

std::string IWWindow::GetActiveApproachName() const
{
    std::lock_guard<std::mutex> lock(approachDataMutex);
    return selectedApproach.title;
}

void IWWindow::SetActiveApproach(const IWApproachDefinition& selectedApproach)
{
    std::lock_guard<std::mutex> lock(approachDataMutex);
    this->selectedApproach = selectedApproach;
    this->approachLength = selectedApproach.defaultRange;
    this->leftToRight = selectedApproach.localizerCourse > 0 && selectedApproach.localizerCourse < 180;
    this->titleBar.SetTitle(selectedApproach.title);
    UpdateDimentions();
    Invalidate();
}

void IWWindow::CreatePopupMenu(CPoint point)
{
    // Create the main popup menu
    CMenu menu;
    menu.CreatePopupMenu();

    // Create the submenu with the available approaches
    CMenu subMenuSelect;
    CMenu subMenuOpenNew;
    subMenuSelect.CreatePopupMenu();
    subMenuOpenNew.CreatePopupMenu();

    int idCounter = 0;
    for (const IWApproachDefinition& approach : availableApproaches)
    {
        bool isActive = approach.title == this->selectedApproach.title;
        int menuItemID = idCounter++;
        if (isActive) {
            subMenuSelect.AppendMenu(MF_STRING | MF_CHECKED, MENU_ITEM_PROCEDURES_SEL_START + menuItemID, CString(approach.title.c_str()));
        }
        else {
            subMenuSelect.AppendMenu(MF_STRING, MENU_ITEM_PROCEDURES_SEL_START + menuItemID, CString(approach.title.c_str()));
        }
        subMenuOpenNew.AppendMenu(MF_STRING, MENU_ITEM_PROCEDURES_NEW_START + menuItemID, CString(approach.title.c_str()));
    }

    menu.AppendMenu(MF_POPUP, (UINT_PTR)subMenuSelect.m_hMenu, _T("View"));
    menu.AppendMenu(MF_POPUP, (UINT_PTR)subMenuOpenNew.m_hMenu, _T("Open"));

    // Add static menu items
    menu.AppendMenu(
        MF_STRING | (this->showTagsByDefault ? MF_CHECKED : MF_UNCHECKED),
        MENU_ITEM_SHOW_LABELS,
        _T("Show labels by default")
    );
    menu.AppendMenu(
        MF_STRING | (this->applyTemperatureCorrection ? MF_CHECKED : MF_UNCHECKED),
        MENU_ITEM_CORRECT_FOR_TEMPERATURE,
        _T("Apply temperature correction")
    );
    menu.AppendMenu(
        MF_STRING,
        MENU_ITEM_FLIP,
        _T("Change orientation")
    );

    // Display the menu
    menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
    
}

BOOL IWWindow::OnMenuOptionSelected(UINT nID)
{
    if (nID == MENU_ITEM_FLIP)
    {
        this->leftToRight = !this->leftToRight;
        UpdateDimentions();
        Invalidate();
    }
    else if (nID == MENU_ITEM_SHOW_LABELS)
    {
        this->showTagsByDefault = !this->showTagsByDefault;
        Invalidate();
    }
    else if (nID == MENU_ITEM_CORRECT_FOR_TEMPERATURE)
    {
        this->applyTemperatureCorrection = !this->applyTemperatureCorrection;
        Invalidate();
    }
    return TRUE;
}

void IWWindow::OnProcedureSelected(UINT nID)
{
    int index = nID - MENU_ITEM_PROCEDURES_SEL_START;
    if (index >= 0 && index < availableApproaches.size())
    {
        // Set the selected approach
        IWApproachDefinition selectedApproach = availableApproaches[index];
        SetActiveApproach(selectedApproach);
    }
    else
    {
        // Open a new window with the selected approach
        index = nID - MENU_ITEM_PROCEDURES_NEW_START;
        if (index >= 0 && index < availableApproaches.size())
        {
            IWApproachDefinition selectedApproach = availableApproaches[index];
            m_listener->OnWindowMenuOpenNew(selectedApproach.title);
        }
    }
    Invalidate();
}

