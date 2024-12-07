#include "pch.h"
#include "WindowTitleBar.h"

IMPLEMENT_DYNAMIC(WindowTitleBar, CStatic)

BEGIN_MESSAGE_MAP(WindowTitleBar, CStatic)
    ON_WM_PAINT()
    ON_WM_SIZE()  // Handle resizing
    ON_BN_CLICKED(IDC_CLOSE_BUTTON, &WindowTitleBar::OnCloseButtonClicked)
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

WindowTitleBar::WindowTitleBar(std::string text, COLORREF backgroundColor, COLORREF textColor)
{
    this->backgroundColor = backgroundColor;
    this->textColor = textColor;
}

BOOL WindowTitleBar::CreateTopBar(CWnd* pParentWnd, const CRect& rect, UINT nID)
{
    if (!CWnd::Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect, pParentWnd, nID))
        return FALSE;

    // Create the close button
    CRect closeButtonRect(rect.right - 40, rect.top + 5, rect.right - 5, rect.top + 25);
    if (!m_CloseButton.Create(_T("X"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, closeButtonRect, this, IDC_CLOSE_BUTTON))
        return FALSE;

    m_CloseButton.SetFont(pParentWnd->GetFont());
    return TRUE;
}

void WindowTitleBar::OnPaint()
{
    CPaintDC dc(this);  // Device context for painting
    CRect rect;
    GetClientRect(&rect);  // Get the client area of the control

    // Fill the background with your custom color
    dc.FillSolidRect(rect, this->backgroundColor);  // Dark background

    // Set the text color to white
    dc.SetTextColor(this->textColor);
    dc.SetBkMode(TRANSPARENT);  // Transparent background for text

    // Draw the text centered in the client area
    dc.DrawText(_T("ILS 01L"), -1, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

void WindowTitleBar::OnCloseButtonClicked()
{
    GetParent()->PostMessage(WM_CLOSE);
}


void WindowTitleBar::OnLButtonDown(UINT nFlags, CPoint point)
{
    CRect closeButtonRect;
    m_CloseButton.GetWindowRect(&closeButtonRect);
    ScreenToClient(&closeButtonRect);

    if (!closeButtonRect.PtInRect(point))  // If click is not on the close button
    {
        // Simulate dragging the window
        CWnd* pParent = GetParent();
        if (pParent)
        {
            pParent->SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
        }
    }

    CWnd::OnLButtonDown(nFlags, point);
}

void WindowTitleBar::OnSize(UINT nType, int cx, int cy)
{
    CStatic::OnSize(nType, cx, cy);

    if (m_CloseButton.GetSafeHwnd())
    {
        // Adjust the position of the close button to stay at the right
        CRect rect;
        GetClientRect(&rect);  // Get the updated size of the title bar
        CRect closeButtonRect(rect.right - 40, rect.top + 5, rect.right - 5, rect.top + 25);
        m_CloseButton.MoveWindow(closeButtonRect);
    }
}
