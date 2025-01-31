#include "pch.h"
#include "IWX11MenuBtn.h"


void IWX11MenuBtn::DrawSymbol(CDC* pDC, CRect rect)
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

    // Select a brush to fill the triangle
    CBrush brush(RGB(0, 0, 0));  // Black color for the triangle
    CBrush* oldBrush = pDC->SelectObject(&brush);

    // Draw the filled triangle
    pDC->Polygon(points, 3);

    // Restore the previous brush
    pDC->SelectObject(oldBrush);
}
