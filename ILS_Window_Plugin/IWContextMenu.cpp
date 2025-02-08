#include "pch.h"
#include "IWContextMenu.h"

BEGIN_MESSAGE_MAP(IWContextMenu, CWnd)
    ON_WM_PAINT()
END_MESSAGE_MAP()

void IWContextMenu::SubclassMenu(HWND hMenuWnd)
{
    // Attach to the existing menu window
    Attach(hMenuWnd);
}

void IWContextMenu::OnPaint()
{
    CPaintDC dc(this); // Get device context

    // Get the menu window's size
    CRect rect;
    GetClientRect(&rect);

    // Draw the default menu first
    DefWindowProc(WM_PAINT, (WPARAM)dc.m_hDC, 0);

    // Draw custom border
    COLORREF borderColor = RGB(255, 0, 0); // Red border
    int borderWidth = 2;

    CBrush borderBrush(borderColor);
    dc.FrameRect(&rect, &borderBrush);

    // You can also use dc.DrawEdge() for different styles
}
