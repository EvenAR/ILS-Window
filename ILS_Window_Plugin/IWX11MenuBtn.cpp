#include "pch.h"
#include "IWX11MenuBtn.h"

void IWX11MenuBtn::DrawIcon(CDC* pDC, CRect rect, bool focused)
{
    // Calculate the center of the rectangle
    CPoint center = rect.CenterPoint();

    // Calculate the size of the triangle based on the width and height of the rectangle
    int triangleHeight = rect.Height() * 0.7;
    int triangleBase = triangleHeight;

    // Define the points for the triangle, pointing downwards
    CPoint points[3];
    points[0] = CPoint(center.x, center.y + triangleHeight / 2); // Top vertex (now at the bottom)
    points[1] = CPoint(center.x - triangleBase / 2, center.y - triangleHeight / 2); // Bottom left vertex
    points[2] = CPoint(center.x + triangleBase / 2, center.y - triangleHeight / 2); // Bottom right vertex

    // Select a brush and pen
    CBrush* oldBrush = pDC->SelectObject(focused ? &iconFocusBrush : &iconDefaultBrush);
    CPen* oldPen = pDC->SelectObject(focused ? &iconFocusPen : &iconDefaultPen);

    // Draw the filled triangle
    pDC->Polygon(points, 3);

    // Restore the previous brush and pen
    pDC->SelectObject(oldBrush);
    pDC->SelectObject(oldPen);
}
