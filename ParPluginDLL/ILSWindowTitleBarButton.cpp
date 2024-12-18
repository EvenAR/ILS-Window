#include "pch.h"
#include "ILSWindowTitleBarButton.h"

BEGIN_MESSAGE_MAP(ILSWindowTitleBarButton, CButton)
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


BOOL ILSWindowTitleBarButton::OnEraseBkgnd(CDC* pDC) {
    // Do nothing to keep the background transparent
    return TRUE;
}

void ILSWindowTitleBarButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

    // Get the button's rectangle
    CRect rect = lpDrawItemStruct->rcItem;

    // Draw border only (hollow interior)
    CPen pen(PS_SOLID, 1, RGB(0, 0, 0)); // Black pen for the border
    CPen* oldPen = pDC->SelectObject(&pen);
    pDC->SelectStockObject(HOLLOW_BRUSH); // No fill for the rectangle
    pDC->Rectangle(&rect);
    pDC->SelectObject(oldPen);

    // Call DrawSymbol to draw the custom symbol inside
    DrawSymbol(pDC, rect);
}
