#include "pch.h"
#include "IWX11CloseBtn.h"

void IWX11CloseBtn::DrawSymbol(CDC* pDC, CRect rect)
{
    // Draw a black circle in the center
    CBrush brush(RGB(0, 0, 0)); // Solid black brush for the circle
    CBrush* oldBrush = pDC->SelectObject(&brush);

    int radius = min(rect.Width(), rect.Height()) / 3; // Circle radius
    CPoint center = rect.CenterPoint();
    pDC->Ellipse(center.x - radius, center.y - radius, center.x + radius, center.y + radius);

    // Restore the old GDI objects
    pDC->SelectObject(oldBrush);
}
