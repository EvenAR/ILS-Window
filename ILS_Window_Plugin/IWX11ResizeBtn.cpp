#include "pch.h"
#include "IWX11ResizeBtn.h"

void IWX11ResizeBtn::DrawIcon(CDC* pDC, CRect rect) {
    // Create a 1px black pen
    CPen pen(PS_SOLID, 1, this->iconColor);
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
