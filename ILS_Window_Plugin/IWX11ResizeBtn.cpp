#include "pch.h"
#include "IWX11ResizeBtn.h"

BEGIN_MESSAGE_MAP(IWX11ResizeBtn, CButton)
    ON_WM_LBUTTONDOWN()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BOOL IWX11ResizeBtn::OnEraseBkgnd(CDC* pDC)
{
    // Do nothing to keep the background transparent
    return TRUE;
}

void IWX11ResizeBtn::DrawSymbol(CDC* pDC, CRect rect) {
    // Create a 1px black pen
    CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
    CPen* oldPen = pDC->SelectObject(&pen);

    // Draw first small rectangle (1/3 size)
    CRect smallRect1(
        rect.left,
        rect.top,
        rect.left + rect.Width() / 3 + 1,
        rect.top + rect.Height() / 3 + 1
    );
    pDC->Rectangle(smallRect1);

    // Draw second small rectangle (2/3 size)
    CRect smallRect2(
        rect.left,
        rect.top,
        rect.left + rect.Width() * 2 / 3,
        rect.top + rect.Height() * 2 / 3
    );
    pDC->Rectangle(smallRect2);

    // Restore old pen
    pDC->SelectObject(oldPen);
}


void IWX11ResizeBtn::OnLButtonDown(UINT nFlags, CPoint point)
{
    CPoint screenPoint = point;
    ClientToScreen(&screenPoint);

    // Get the parent window
    CWnd* parentWnd = GetParent();
    if (parentWnd)
    {
        parentWnd->SendMessage(WM_LBUTTONDOWN, NULL, MAKELPARAM(screenPoint.x, screenPoint.y));
    }
}