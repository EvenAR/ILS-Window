#include "pch.h"
#include "IWTitleBar.h"
#include "RenderUtils.h"

IMPLEMENT_DYNAMIC(IWTitleBar, CStatic)

BEGIN_MESSAGE_MAP(IWTitleBar, CStatic)
    ON_WM_PAINT()
    ON_WM_SIZE()  // Handle resizing
    ON_BN_CLICKED(IDC_CLOSE_BUTTON, &IWTitleBar::OnCloseButtonClicked)
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

IWTitleBar::IWTitleBar(std::string title, COLORREF backgroundColor, COLORREF textColor, IWTitleBarEventListener* listener)
{
    this->backgroundColor = backgroundColor;
    this->textColor = textColor;

    this->text = title;
    this->font.CreatePointFont(90, _T("EuroScope"));
    this->eventListener = listener;
}

BOOL IWTitleBar::CreateTopBar(CWnd* pParentWnd, const CRect& rect, UINT nID)
{
    if (!CWnd::Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect, pParentWnd, nID))
        return FALSE;

    // Create buttons with default settings
    /*if (!resizeButton.Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect(), this, IDC_RESIZE_BUTTON) ||
        !menuButton.Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect(), this, IDC_MENU_BUTTON) ||
        !closeButton.Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect(), this, IDC_CLOSE_BUTTON))
        return FALSE;

    // Position buttons
    PositionButtons(rect);*/

    return TRUE;
}

void IWTitleBar::OnPaint()
{
    CPaintDC dc(this);  // Device context for painting
    CRect rect;
    GetClientRect(&rect);  // Get the client area of the control

    // Fill the background with your custom color
    dc.FillSolidRect(rect, this->backgroundColor);  // Dark background

    DrawCdeStyleContent(&dc, rect);

}

void IWTitleBar::OnCloseButtonClicked()
{
    this->eventListener->OnCloseButtonClicked();
}


void IWTitleBar::OnLButtonDown(UINT nFlags, CPoint point)
{
    /*CRect resizeButtonRect;
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
    else {*/
        // Simulate dragging the window
        CWnd* pParent = GetParent();
        if (pParent)
        {
            pParent->SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
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
    const int btnHeight = rect.Height() - 9;
    const int btnWidth = btnHeight * 1.2;

    // V center the buttons
    const int top = (rect.Height() - btnHeight) / 2;
    const int bottom = top + btnHeight;

    int right = rect.right - 3;
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

void IWTitleBar::DrawCdeStyleContent(CDC* pdc, CRect rect)
{
    CRect closeButton = rect;
    closeButton.right = closeButton.left + rect.Height();

    CRect minimizeIcon = closeButton;
    minimizeIcon.left = closeButton.left + closeButton.Width() * 0.25;
    minimizeIcon.right = minimizeIcon.right - closeButton.Width() * 0.25;
    minimizeIcon.top = closeButton.top + closeButton.Height() / 2 - 2;
    minimizeIcon.bottom = minimizeIcon.top + 4;

    Draw3dRect(pdc, closeButton);
    Draw3dRect(pdc, minimizeIcon);

    CRect menuButton = rect;
    menuButton.left = rect.right - rect.Height();

    CRect menuIcon = menuButton;
    menuIcon.left = menuButton.left + menuButton.Width() * 0.35;
    menuIcon.right = menuIcon.right - menuButton.Width() * 0.35;
    menuIcon.top = menuButton.top + menuButton.Width() * 0.35;
    menuIcon.bottom = menuButton.bottom - menuButton.Width() * 0.35;

    Draw3dRect(pdc, menuButton);
    Draw3dRect(pdc, menuIcon);

    CRect restOfTheBar = rect;
    restOfTheBar.left = closeButton.right;
    restOfTheBar.right = menuButton.left;

    Draw3dRect(pdc, restOfTheBar);

    // Draw window title
    auto oldFont = pdc->SelectObject(this->font);
    pdc->SetTextColor(this->textColor);
    pdc->SetBkMode(TRANSPARENT);  
    pdc->DrawText(_T(this->text.c_str()), -1, restOfTheBar, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    pdc->SelectObject(oldFont);
}
