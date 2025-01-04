#include "pch.h"
#include "IWTitleBar.h"

IMPLEMENT_DYNAMIC(IWTitleBar, CStatic)

BEGIN_MESSAGE_MAP(IWTitleBar, CStatic)
    ON_WM_PAINT()
    ON_WM_SIZE()  // Handle resizing
    ON_BN_CLICKED(IDC_CLOSE_BUTTON, &IWTitleBar::OnCloseButtonClicked)
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

IWTitleBar::IWTitleBar(std::string title, COLORREF backgroundColor, COLORREF textColor, COLORREF outerFrameColor, IWTitleBarEventListener* listener)
{
    this->backgroundColor = backgroundColor;
    this->textColor = textColor;
    this->outerFramePen.CreatePen(PS_SOLID, 1, outerFrameColor);

    this->text = title;
    this->euroScopeFont.CreatePointFont(110, _T("EuroScope"));
    this->eventListener = listener;
}

BOOL IWTitleBar::CreateTopBar(CWnd* pParentWnd, const CRect& rect, UINT nID)
{
    if (!CWnd::Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect, pParentWnd, nID))
        return FALSE;

    // Create buttons with default settings
    if (!resizeButton.Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect(), this, IDC_RESIZE_BUTTON) ||
        !menuButton.Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect(), this, IDC_MENU_BUTTON) ||
        !closeButton.Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect(), this, IDC_CLOSE_BUTTON))
        return FALSE;

    // Position buttons
    PositionButtons(rect);

    return TRUE;
}

void IWTitleBar::OnPaint()
{
    CPaintDC dc(this);  // Device context for painting
    CRect rect;
    GetClientRect(&rect);  // Get the client area of the control

    auto oldFont = dc.SelectObject(this->euroScopeFont);

    // Fill the background with your custom color
    dc.FillSolidRect(rect, this->backgroundColor);  // Dark background

    // Set the text color to white
    dc.SetTextColor(this->textColor);
    dc.SetBkMode(TRANSPARENT);  // Transparent background for text

    // Draw the text centered in the client area
    CRect textPosition = rect;
    textPosition.left += 10;

    dc.DrawText(_T(this->text.c_str()), -1, textPosition, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    dc.SelectObject(oldFont);

    dc.SelectStockObject(NULL_BRUSH);
    dc.SelectObject(this->outerFramePen);
    rect.bottom += 1; // Make the bottom border invisible
    dc.Rectangle(rect);
}

void IWTitleBar::OnCloseButtonClicked()
{
    this->eventListener->OnCloseButtonClicked();
}


void IWTitleBar::OnLButtonDown(UINT nFlags, CPoint point)
{
    CRect resizeButtonRect;
    resizeButton.GetClientRect(&resizeButtonRect);
    resizeButton.ClientToScreen(&resizeButtonRect);

    CRect menuButtonRect;
    menuButton.GetClientRect(&menuButtonRect);
    menuButton.ClientToScreen(&menuButtonRect);

    if (resizeButtonRect.PtInRect(point))  // If click is not on the close button
    {
        this->eventListener->OnResizeStart();
    }
    else if (menuButtonRect.PtInRect(point))
    {
        this->eventListener->OnMenuButtonClicked();
    }
    else {
        // Simulate dragging the window
        CWnd* pParent = GetParent();
        if (pParent)
        {
            pParent->SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
        }
    }

    CWnd::OnLButtonDown(nFlags, point);
}

void IWTitleBar::OnSize(UINT nType, int cx, int cy)
{
    CStatic::OnSize(nType, cx, cy);

    if (closeButton.GetSafeHwnd() && menuButton.GetSafeHwnd() && resizeButton.GetSafeHwnd())
    {
        CRect rect;
        GetClientRect(&rect);  // Get the updated size of the title bar
        PositionButtons(rect);
    }
}

void IWTitleBar::PositionButtons(const CRect& rect)
{
    const int margin = 6;
    const int btnWidth = 16; // Button width
    const int btnHeight = 14; // Button width
    const int top = rect.top + 8;
    const int bottom = top + btnHeight;

    int right = rect.right - margin - 3;
    int left = right - btnWidth;

    // Position the resize button
    CRect resizeButtonRect(left, top, right, bottom);
    if (resizeButton.GetSafeHwnd())
        resizeButton.MoveWindow(resizeButtonRect);

    right = left - margin;
    left = right - btnWidth;

    // Position the menu button
    CRect menuButtonRect(left, top, right, bottom);
    if (menuButton.GetSafeHwnd())
        menuButton.MoveWindow(menuButtonRect);

    right = left - margin;
    left = right - btnWidth;

    // Position the close button
    CRect closeButtonRect(left, top, right, bottom);
    if (closeButton.GetSafeHwnd())
        closeButton.MoveWindow(closeButtonRect);
}
