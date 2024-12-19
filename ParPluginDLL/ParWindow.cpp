#include "pch.h"
#include "ParWindow.h"

BEGIN_MESSAGE_MAP(ParWindow, CWnd)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_LBUTTONDOWN()   // Add the message map entry for WM_LBUTTONDOWN
    ON_WM_LBUTTONUP()     // Add the message map entry for WM_LBUTTONUP
    ON_WM_CTLCOLOR() // Handle custom control colors
    ON_MESSAGE(WM_UPDATE_DATA, &ParWindow::OnUpdateData)
    ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
    ON_WM_NCACTIVATE()
    ON_WM_GETMINMAXINFO()
    ON_WM_MOUSEWHEEL()
    ON_WM_TIMER()
END_MESSAGE_MAP()

ParWindow::ParWindow(const char* title, double appSlope, double appLength, bool leftToRight, float maxOffsetLeft, float maxOffsetRight, ParStyling styling) : titleBar(
    title,
    RGB(styling.windowFrameColor.r, styling.windowFrameColor.g, styling.windowFrameColor.b),
    RGB(styling.windowFrameTextColor.r, styling.windowFrameTextColor.g, styling.windowFrameTextColor.b),
    RGB(styling.windowOuterFrameColor.r, styling.windowOuterFrameColor.g, styling.windowOuterFrameColor.b),
    this
)
{
    this->approachSlope = appSlope;
    this->approachLength = appLength;
    this->leftToRight = leftToRight;
    this->maxOffsetLeft = maxOffsetLeft;
    this->maxOffsetRight = maxOffsetRight;

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

ParWindow::~ParWindow()
{
    this->titleBar.DestroyWindow();
}

int ParWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
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


void ParWindow::OnPaint()
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

void ParWindow::DrawContent(CDC& dc)
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
    for (const ParRadarTarget& radarTarget : m_latestParData.radarTargets)
    {
        auto it = radarTarget.positionHistory.rbegin();
        auto end = radarTarget.positionHistory.rend();

        // Draw the history trail in reverese so the newest dots appear on top
        for (auto it = radarTarget.positionHistory.rbegin(); it != radarTarget.positionHistory.rend(); ++it) {
            const ParTargetPosition& position = *it;
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
                if (this->tagMode == ParTagMode::Squawk) {
                    targetLabel.Format(_T("%s"), radarTarget.squawk.c_str());
                }
                else if (this->tagMode == ParTagMode::Callsign) {
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

void ParWindow::OnSize(UINT nType, int cx, int cy)
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

BOOL ParWindow::OnNcActivate(BOOL bActive)
{
    // Prevent Windows from redrawing the NC area
    return TRUE;
}

BOOL ParWindow::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CWnd::PreCreateWindow(cs))
        return FALSE;

    return TRUE;
}

BOOL ParWindow::OnEraseBkgnd(CDC* pDC)
{
    // Do nothing here to prevent background clearing
    return TRUE;
}

LRESULT ParWindow::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
    ParData* pData = reinterpret_cast<ParData*>(wParam);
    if (pData) {
        m_latestParData = *pData;
    }

    // Trigger a repaint
    CRect updateRect = GetClientRectBelowTitleBar(); // Adjust based on what needs redrawing
    InvalidateRect(&updateRect, FALSE); // FALSE to avoid erasing the background unnecessarily

    return 0;
}


CRect ParWindow::GetClientRectBelowTitleBar()
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

void ParWindow::DrawDiamond(CPoint pt, int radius, CDC& dc)
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

void ParWindow::OnResizeStart()
{
    SendMessage(WM_NCLBUTTONDOWN, HTTOPRIGHT, NULL); // Resize using the top right corner
}

void ParWindow::OnCloseButtonClicked()
{
    this->DestroyWindow();
}

void ParWindow::OnDestroy()
{
    if (m_listener) {
        m_listener->OnWindowClosed(this);
    }
    // Call base class cleanup (this ensures MFC cleans up the window itself)
    CWnd::OnDestroy();
}

void ParWindow::SetListener(IParWindowEventListener* listener)
{
    m_listener = listener;
}

void ParWindow::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize.x = 150; // Minimum width in pixels
    lpMMI->ptMinTrackSize.y = 100; // Minimum height in pixels
}

BOOL ParWindow::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
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

void ParWindow::OnTimer(UINT_PTR nIDEvent)
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

void ParWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
    for (const ParRadarTarget& radarTarget : m_latestParData.radarTargets) {
        auto newestPosition = radarTarget.positionHistory.size() > 0 ? &radarTarget.positionHistory.front() : nullptr;

        if (newestPosition) {
            const ParTargetPosition& position = *newestPosition;

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

bool ParWindow::CalculateTargetCoordinates(const ParTargetPosition& position, CPoint& ptTopView, CPoint& ptSideView)
{
    double angleDiff = position.directionToThreshold / 180.0 * PI; // anglediff in radians
    double projectedDistanceFromThreshold = position.distanceToThreshold * cos(angleDiff);
    double projectedDistanceFromExtendedCenterline = position.distanceToThreshold * tan(angleDiff);
    if (projectedDistanceFromExtendedCenterline < 0 && abs(projectedDistanceFromExtendedCenterline) > this->maxOffsetLeft) {
        // Too far left
        return false;
    }
    if (projectedDistanceFromExtendedCenterline > 0 && abs(projectedDistanceFromExtendedCenterline) > this->maxOffsetRight) {
        // Too far right
        return false;
    }
    if (projectedDistanceFromThreshold < 0) {
        // Wrong side of the threshold
        return false;
    }
    if (position.heightAboveThreshold > approachHeightFt) {
        // Too high
        return false;
    }
    int xPosition = glidePathBottom.x - projectedDistanceFromThreshold * pixelsPerNauticalMile;
    int yPositionSlope = glidePathBottom.y - position.heightAboveThreshold * pixelsPerFt;
    int yPositionCenterline = glidePathBottom.y + projectedDistanceFromExtendedCenterline * pixelsPerNauticalMile;
    ptSideView = CPoint(xPosition, yPositionSlope);
    ptTopView = CPoint(xPosition, yPositionCenterline);
    return true;
}

void ParWindow::UpdateDimentions()
{
    CRect rect = GetClientRectBelowTitleBar();

    const int CALC_SIDE_MARGIN = rect.Width() * APP_LINE_MARGIN_SIDES;
    const int CALC_TOP_MARGIN = rect.Height() * APP_LINE_MARGIN_TOP;
    const int CALC_BOTTOM_MARGIN = rect.Height() * APP_LINE_MARGIN_BOTTOM;

    // Define the start and end coordintes for rendering the glidepath
    this->glidePathTop = CPoint(leftToRight ? rect.left + CALC_SIDE_MARGIN : rect.right - CALC_SIDE_MARGIN, rect.top + CALC_TOP_MARGIN);
    this->glidePathBottom = CPoint(leftToRight ? rect.right - CALC_SIDE_MARGIN : rect.left + CALC_SIDE_MARGIN, rect.bottom - CALC_BOTTOM_MARGIN);

    this->approachHeightFt = (approachLength * FT_PER_NM * sin(approachSlope / 180.0 * PI));

    this->pixelsPerFt = (glidePathBottom.y - glidePathTop.y) / approachHeightFt;
    this->pixelsPerNauticalMile = (glidePathBottom.x - glidePathTop.x) / float(approachLength); // PS: negative when direction is left->right
}
