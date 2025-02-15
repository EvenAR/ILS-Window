#include "pch.h"
#include "IWX11IconifyBtn.h"

void IWX11IconifyBtn::DrawIcon(CDC* pDC, CRect rect, bool focused)
{
    // Draw a black circle in the center
    CBrush brush(this->iconColor); // Solid black brush for the circle
    CBrush* oldBrush = pDC->SelectObject(&brush);
    CPen pen(PS_SOLID, 1, this->iconColor);
    CPen* oldPen = pDC->SelectObject(&pen);

    int radius = min(rect.Width(), rect.Height()) / 3; // Circle radius
    CPoint center = rect.CenterPoint();
    pDC->Ellipse(center.x - radius, center.y - radius, center.x + radius, center.y + radius);

    // Restore the old GDI objects
    pDC->SelectObject(oldBrush);
    pDC->SelectObject(oldPen);
}
