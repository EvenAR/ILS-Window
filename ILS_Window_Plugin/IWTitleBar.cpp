#include "pch.h"
#include "IWTitleBar.h"
#include "IWTitleBarBtn.h"

IMPLEMENT_DYNAMIC(IWTitleBar, CStatic)

BEGIN_MESSAGE_MAP(IWTitleBar, CStatic)
    ON_WM_PAINT()
    ON_WM_SIZE()  // Handle resizing
    ON_WM_LBUTTONDOWN()
    ON_BN_CLICKED(IDC_CLOSE_BUTTON, &IWTitleBar::OnCloseButtonClicked)
    ON_BN_CLICKED(IDC_MENU_BUTTON, &IWTitleBar::OnMenuButtonClicked)
END_MESSAGE_MAP()

IWTitleBar::IWTitleBar(std::string title, COLORREF backgroundColor, int fontSize, IWTitleBarEventListener* listener)
{
    this->backgroundColor = backgroundColor;
    this->text = title;

    float fontPointsSize = fontSize * 72 / 96;
    this->font.CreatePointFont(int(fontPointsSize * 10), _T("EuroScope"));
    this->eventListener = listener;
}

BOOL IWTitleBar::CreateTopBar(CWnd* pParentWnd, const CRect& rect, UINT nID)
{
    if (!CWnd::Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect, pParentWnd, nID))
        return FALSE;

    // Create buttons with default settings
    if (!resizeButton->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect(), this, IDC_RESIZE_BUTTON) ||
        !menuButton->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect(), this, IDC_MENU_BUTTON) ||
        !closeButton->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect(), this, IDC_CLOSE_BUTTON))
        return FALSE;

    // Position buttons
    PositionButtons(rect);

    return TRUE;
}

void IWTitleBar::OnSize(UINT nType, int cx, int cy)
{
    CStatic::OnSize(nType, cx, cy);

    if (closeButton->GetSafeHwnd() && menuButton->GetSafeHwnd() && resizeButton->GetSafeHwnd())
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
    DrawTitle(&dc, rect);
}

void IWTitleBar::OnLButtonDown(UINT nFlags, CPoint point)
{
    CWnd* pParent = GetParent();
    if (!pParent)
        return;

    CRect menuButtonRect;
    menuButton->GetClientRect(&menuButtonRect);
    menuButton->ClientToScreen(&menuButtonRect);

    CRect resizeButtonRect;
    resizeButton->GetClientRect(&resizeButtonRect);
    resizeButton->ClientToScreen(&resizeButtonRect);

    if (resizeButtonRect.PtInRect(point)) {
        // Start resizing the window
        this->eventListener->OnResizeStart();
    }
    else if (menuButtonRect.PtInRect(point)) {
        // Simulate clicking the menu button
        this->eventListener->OnMenuButtonClicked();
    }
    else {
        // Simulate dragging the window
        pParent->SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
    }

    CWnd::OnLButtonDown(nFlags, point);
}

void IWTitleBar::OnCloseButtonClicked()
{
    this->eventListener->OnCloseButtonClicked();
}

void IWTitleBar::OnMenuButtonClicked()
{
    this->eventListener->OnMenuButtonClicked();
}

