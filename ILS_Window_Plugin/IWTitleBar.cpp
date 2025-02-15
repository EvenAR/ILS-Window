#include "pch.h"
#include "IWTitleBar.h"
#include "IWTitleBarBtn.h"

IMPLEMENT_DYNAMIC(IWTitleBar, CStatic)

BEGIN_MESSAGE_MAP(IWTitleBar, CStatic)
    ON_WM_PAINT()
    ON_WM_SIZE()  // Handle resizing
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_BN_CLICKED(IDC_CLOSE_BUTTON, &IWTitleBar::OnIconifyButtonClicked)
    ON_BN_CLICKED(IDC_MENU_BUTTON, &IWTitleBar::OnMenuButtonClicked)
    ON_COMMAND(IDC_RESIZE_BUTTON, &IWTitleBar::OnResizeButtonPressed)
END_MESSAGE_MAP()

IWTitleBar::IWTitleBar(COLORREF backgroundColor, int fontSize, IWTitleBarEventListener* listener)
{
    this->backgroundColor = backgroundColor;

    float fontPointsSize = fontSize * 72 / 96;
    this->mainFont.CreatePointFont(int(fontPointsSize * 10), _T("EuroScope"));
    this->eventListener = listener;
}

BOOL IWTitleBar::CreateTopBar(CWnd* pParentWnd, const CRect& rect, UINT nID)
{
    if (!CWnd::Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect, pParentWnd, nID))
        return FALSE;

    // Create buttons with default settings
    if (!resizeButton->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect(), this, IDC_RESIZE_BUTTON) ||
        !menuButton->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect(), this, IDC_MENU_BUTTON) ||
        !iconifyButton->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect(), this, IDC_CLOSE_BUTTON))
        return FALSE;

    resizeButton->SetButtonID(IDC_RESIZE_BUTTON);

    // Position buttons
    PositionButtons(rect);

    return TRUE;
}

void IWTitleBar::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    if (iconifyButton->GetSafeHwnd() && menuButton->GetSafeHwnd() && resizeButton->GetSafeHwnd())
    {
        CRect rect;
        GetClientRect(&rect);  // Get the updated size of the title bar
        PositionButtons(rect);
    }
}

void IWTitleBar::OnPaint()
{
    CPaintDC dc(this);
    CRect rect;
    GetClientRect(&rect);

    // Draw background
    dc.FillSolidRect(rect, backgroundColor);

    // Get parent window
    CString title;
    GetParent()->GetWindowText(title);
    DrawTitle(&dc, rect, title, isBeingDragged);
}

void IWTitleBar::OnLButtonDown(UINT nFlags, CPoint point)
{
    CWnd* pParent = GetParent();
    if (!pParent)
        return;

    CRect resizeButtonRect;
    resizeButton->GetClientRect(&resizeButtonRect);
    resizeButton->ClientToScreen(&resizeButtonRect);

    if (resizeButtonRect.PtInRect(point)) {
        this->eventListener->OnResizeStart();
    }
    else {
        if (!isBeingDragged)
        {
            StartDragging(point);
        }

        // Forward the message to the parent to start dragging the window
        CWnd* pParent = GetParent();
        if (pParent)
        {
            // Forward the message to the parent so it knows it's being dragged
            pParent->SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
        }
    }

    CWnd::OnLButtonDown(nFlags, point);
}

void IWTitleBar::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (isBeingDragged)
    {
        StopDragging();
    }

    // Forward the message to the parent to stop dragging the window
    CWnd* pParent = GetParent();
    if (pParent)
    {
        pParent->SendMessage(WM_NCLBUTTONUP, HTCAPTION, MAKELPARAM(point.x, point.y));
    }

    CWnd::OnLButtonUp(nFlags, point);
}

void IWTitleBar::OnIconifyButtonClicked()
{
    this->eventListener->OnIconifyButtonClicked();
}

void IWTitleBar::OnMenuButtonClicked()
{
    this->eventListener->OnMenuButtonClicked();
}

void IWTitleBar::OnResizeButtonPressed()
{
    this->eventListener->OnResizeStart();
}

void IWTitleBar::OnMouseMove(UINT nFlags, CPoint point)
{
    if (isBeingDragged)
    {
        CPoint screenPos = point;
        ClientToScreen(&screenPos);
        HandleMouseMove(screenPos);
    }

    CWnd::OnMouseMove(nFlags, point);
}


void IWTitleBar::StartDragging(CPoint point)
{
    lastPoint = { -1, -1 };

    isBeingDragged = TRUE;

    // Capture mouse to track movement outside of the window
    SetCapture();

    // Optionally change background color to show it's being pressed
    Invalidate();
}

void IWTitleBar::StopDragging()
{
    if (isBeingDragged)
    {
        isBeingDragged = FALSE;

        // Release the mouse capture
        ReleaseCapture();

        // Optionally reset background color
        Invalidate();

        this->lastPoint = CPoint(-1, -1);
    }
}

void IWTitleBar::HandleMouseMove(CPoint point)
{
    CWnd* pParent = GetParent();
    if (pParent && lastPoint.x != -1 && lastPoint.y != -1)
    {
        // Compute the position of the parent relative to the screen
        CRect parentRect;
        pParent->GetWindowRect(parentRect);

        // Move the parent window by calculating the new position based on the mouse
        int newLeft = parentRect.left + (point.x - lastPoint.x);  // Relative to title bar
        int newTop = parentRect.top + (point.y - lastPoint.y);    // Relative to title bar

        // Avoid unnecessary movement if the new position is the same as the current position
        if (newLeft != parentRect.left || newTop != parentRect.top)
        {
            pParent->SetWindowPos(
                NULL,
                newLeft,
                newTop,
                0,
                0,
                SWP_NOZORDER | SWP_NOSIZE | SWP_NOREDRAW
            );
        }
    }

    // Update lastPoint for the next move
    lastPoint = point;
}