#include "pch.h"
#include "ParWindow.h"

BEGIN_MESSAGE_MAP(ParWindow, CWnd)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_LBUTTONDOWN()   // Add the message map entry for WM_LBUTTONDOWN
    ON_WM_LBUTTONUP()     // Add the message map entry for WM_LBUTTONUP
    ON_WM_CTLCOLOR() // Handle custom control colors
    ON_WM_NCCALCSIZE()
    ON_WM_NCPAINT()
    ON_MESSAGE(WM_UPDATE_DATA, &ParWindow::OnUpdateData)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

ParWindow::ParWindow(const char* title, double appSlope, double appLength, bool leftToRight, ParStyling styling) : titleBar(
    title,
    RGB(styling.titleBarBackgroundColor.r, styling.titleBarBackgroundColor.g, styling.titleBarBackgroundColor.b),
    RGB(styling.titleBarTextColor.r, styling.titleBarTextColor.g, styling.titleBarTextColor.b)
)
{
    this->approachSlope = appSlope;
    this->approachLength = appLength;
    this->leftToRight = leftToRight;

    this->windowBackground = RGB(styling.backgroundColor.r, styling.backgroundColor.g, styling.backgroundColor.b);
    this->targetLabelColor = RGB(styling.targetLabelColor.r, styling.targetLabelColor.g, styling.targetLabelColor.b);
    this->glideSlopePen.CreatePen(PS_SOLID, 1, RGB(styling.glideslopeColor.r, styling.glideslopeColor.g, styling.glideslopeColor.b));
    this->localizerBrush.CreateSolidBrush(RGB(styling.localizerColor.r, styling.localizerColor.g, styling.localizerColor.b));
    this->radarTargetPen.CreatePen(PS_SOLID, 1, RGB(styling.radarTargetColor.r, styling.radarTargetColor.g, styling.radarTargetColor.b));
    this->historyTrailPen.CreatePen(PS_SOLID, 1, RGB(styling.historyTrailColor.r, styling.historyTrailColor.g, styling.historyTrailColor.b));
}

ParWindow::~ParWindow()
{
    this->titleBar.DestroyWindow();
}

int ParWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    SetWindowPos(nullptr, 0, 0, 400, 250, SWP_NOMOVE | SWP_NOZORDER);

    CRect barRect(0, 0, lpCreateStruct->cx, 30);
    if (!titleBar.CreateTopBar(this, barRect, IDC_TOPBAR))
    {
        AfxMessageBox(_T("Failed to create top bar"));
        return -1;  // Handle error appropriately
    }

    titleBar.SetFont(GetFont());
    

    return 0;
}


void ParWindow::OnPaint()
{
    CPaintDC dc(this); // Device context for painting
    CRect rect = GetClientRectBelowTitleBar();
    dc.FillSolidRect(rect, windowBackground);

    double approachHeightFt = (approachLength * FT_PER_NM * sin(approachSlope / 180.0 * PI));

    int APP_LINE_MARGIN_TOP = 30;
    int APP_LINE_MARGIN_SIDES = 30;
    int APP_LINE_MARGIN_BOTTOM = 60;

    // Define the start and end coordintes for rendering the glidepath
    CPoint glidePathTop{ 
        leftToRight ? rect.left + APP_LINE_MARGIN_SIDES : rect.right - APP_LINE_MARGIN_SIDES,
        rect.top + APP_LINE_MARGIN_TOP 
    };
    CPoint glidePathBottom{ 
        leftToRight ? rect.right - APP_LINE_MARGIN_SIDES : rect.left + APP_LINE_MARGIN_SIDES,
        rect.bottom - APP_LINE_MARGIN_BOTTOM 
    };

    // Draw glideslope line
    CPen* pOldPen = dc.SelectObject(&glideSlopePen);
    dc.MoveTo(glidePathTop);
    dc.LineTo(glidePathBottom);
    dc.SelectObject(pOldPen);

    double pixelsPerFt = (glidePathBottom.y - glidePathTop.y) / approachHeightFt;
    double pixelsPerNauticalMile = (glidePathBottom.x - glidePathTop.x) / approachLength; // PS: negative when direction is left->right

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

            if (position.heightAboveThreshold > 10000 || abs(position.directionToThreshold) > 30) continue;

            double angleDiff = position.directionToThreshold / 180.0 * PI; // anglediff in radians
            double projectedDistanceFromThreshold = position.distanceToThreshold * cos(angleDiff); 
            double projectedDistanceFromExtendedCenterline = position.distanceToThreshold * tan(angleDiff);

            int xPosition = glidePathBottom.x - projectedDistanceFromThreshold * pixelsPerNauticalMile;
            int yPositionSlope = glidePathBottom.y - position.heightAboveThreshold * pixelsPerFt;
            int yPositionCenterline = glidePathBottom.y + projectedDistanceFromExtendedCenterline * pixelsPerNauticalMile;

            CPoint ptSideView(xPosition, yPositionSlope);
            CPoint ptTopView(xPosition, yPositionCenterline);

            int crossRadius = isNewestPosition ? TARGET_RADIUS : HISTORY_TRAIL_RADIUS;

            // Draw position seen from the side
            dc.SelectObject(isNewestPosition ? radarTargetPen : historyTrailPen);
            dc.MoveTo(CPoint(ptSideView.x, ptSideView.y - crossRadius));
            dc.LineTo(CPoint(ptSideView.x, ptSideView.y + crossRadius));
            dc.MoveTo(CPoint(ptSideView.x - crossRadius, ptSideView.y));
            dc.LineTo(CPoint(1 + ptSideView.x + crossRadius, ptSideView.y));

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

            if (isNewestPosition) {
                dc.SelectStockObject(NULL_BRUSH);
                dc.Ellipse(ptTopView.x - crossRadius, ptTopView.y - crossRadius, ptTopView.x + crossRadius + 1, ptTopView.y + crossRadius + 1);

                // Callsign label
                dc.SetTextColor(targetLabelColor);
                dc.SetBkMode(TRANSPARENT);
                CString targetLabel(radarTarget.callsign.c_str());
                dc.TextOut(ptTopView.x + crossRadius + 5, ptTopView.y - crossRadius + 10, targetLabel);
            }
        }
    }

}


void ParWindow::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    // Recalculate gradient or redraw on resize
    if (nType != SIZE_MINIMIZED)  // Ignore if window is minimized
    {
        Invalidate(); // Mark the entire client area for repaint
    }

    if (titleBar.GetSafeHwnd())
    {
        CRect barRect(0, 0, cx, 30); // Adjust height as needed
        titleBar.MoveWindow(barRect);
    }
}

BOOL ParWindow::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CWnd::PreCreateWindow(cs))
        return FALSE;

    // Set initial window styles similar to CreateWindowEx
    cs.dwExStyle |= WS_EX_TOPMOST; // Topmost window
    cs.style = WS_POPUP | WS_SIZEBOX; // Custom styles
    cs.cx = 600; // Initial width
    cs.cy = 400; // Initial height
    cs.x = CW_USEDEFAULT; // Default X position
    cs.y = CW_USEDEFAULT; // Default Y position

    return TRUE;
}

LRESULT ParWindow::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
    ParData* pData = reinterpret_cast<ParData*>(wParam);
    if (pData) {
        m_latestParData = *pData;
    }

    // Trigger a repaint
    Invalidate();

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

