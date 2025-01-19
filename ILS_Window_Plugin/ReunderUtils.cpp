#include "RenderUtils.h"

void Draw3dRect(CDC* pDC, CRect rect)
{
    // Colors for the sWunken effectrgb(150, 249, 252)
    COLORREF lightColor = RGB(150, 249, 252); // Lighter shade for top/left
    COLORREF darkColor = RGB(60, 111, 119);  // Darker shade for bottom/right

    for (int drawnSunkSteps = 0; drawnSunkSteps < 2; drawnSunkSteps++) {
        // Draw the top-left border (light color)
        pDC->FillSolidRect(rect.left, rect.top, rect.Width(), 1, lightColor); // Top
        pDC->FillSolidRect(rect.left, rect.top, 1, rect.Height(), lightColor); // Left

        // Draw the bottom-right border (dark color)
        pDC->FillSolidRect(rect.right - 1, rect.top, 1, rect.Height(), darkColor); // Right
        pDC->FillSolidRect(rect.left, rect.bottom - 1, rect.Width(), 1, darkColor); // Bottom

        rect.DeflateRect(1, 1);
    }
}

void Draw3dCorner(CDC* pDC, CRect rect, int borderWidth, COLORREF lightColor, COLORREF darkColor, bool isTop, bool isLeft)
{
    CPen lightPen(PS_SOLID, 1, lightColor);
    CPen darkPen(PS_SOLID, 1, darkColor);

    for (int drawnSunkSteps = 0; drawnSunkSteps < 2; drawnSunkSteps++) {
        if (isTop && isLeft)
        {
            // Light color
            pDC->SelectObject(&lightPen);
            pDC->MoveTo(rect.left, rect.bottom);
            pDC->LineTo(rect.left, rect.top);
            pDC->LineTo(rect.right, rect.top);

            // Dark color
            pDC->SelectObject(&darkPen);
            pDC->MoveTo(rect.right, rect.top);
            pDC->LineTo(rect.right, rect.top + borderWidth - drawnSunkSteps * 2 - 2);
            pDC->LineTo(rect.left + borderWidth - drawnSunkSteps * 2 - 2, rect.top + borderWidth - drawnSunkSteps * 2 - 2);
            pDC->LineTo(rect.left + borderWidth - drawnSunkSteps * 2 - 2, rect.bottom);
            pDC->LineTo(rect.left, rect.bottom);
        }
        else if (isTop && !isLeft)
        {
            // Light color
            pDC->SelectObject(&lightPen);
            pDC->MoveTo(rect.left, rect.top + borderWidth - drawnSunkSteps * 2 - 2);
            pDC->LineTo(rect.left, rect.top);
            pDC->LineTo(rect.right, rect.top);

            pDC->MoveTo(rect.right - borderWidth + drawnSunkSteps * 2 + 2, rect.bottom);
            pDC->LineTo(rect.right - borderWidth + drawnSunkSteps * 2 + 2, rect.top + borderWidth - drawnSunkSteps * 2 - 2);

            // Dark color
            pDC->SelectObject(&darkPen);
            pDC->MoveTo(rect.right, rect.top);
            pDC->LineTo(rect.right, rect.bottom);
            pDC->LineTo(rect.right - borderWidth + drawnSunkSteps * 2 + 2, rect.bottom);

            pDC->MoveTo(rect.right - borderWidth + drawnSunkSteps * 2 + 2, rect.top + borderWidth - drawnSunkSteps * 2 - 2);
            pDC->LineTo(rect.left, rect.top + borderWidth - drawnSunkSteps * 2 - 2);
        }
        else if (!isTop && !isLeft) // Bottom right
        {
            // Light color
            pDC->SelectObject(&lightPen);
            pDC->MoveTo(rect.left, rect.bottom);
            pDC->LineTo(rect.left, rect.top);
            pDC->LineTo(rect.right, rect.top);

            pDC->MoveTo(rect.left, rect.bottom - borderWidth + drawnSunkSteps * 2 + 2);
            pDC->LineTo(rect.right - borderWidth + drawnSunkSteps * 2 + 2, rect.bottom - borderWidth + drawnSunkSteps * 2 + 2);
            pDC->LineTo(rect.right - borderWidth + drawnSunkSteps * 2 + 2, rect.top);

            // Dark color
            pDC->SelectObject(&darkPen);
            pDC->MoveTo(rect.right, rect.top);
            pDC->LineTo(rect.right, rect.bottom);
            pDC->LineTo(rect.left, rect.bottom);
        }
        else if (!isTop && isLeft) // Bottom left
        {
            // Light color
            pDC->SelectObject(&lightPen);
            pDC->MoveTo(rect.left, rect.bottom);
            pDC->LineTo(rect.left, rect.top);
            pDC->LineTo(rect.left + borderWidth - drawnSunkSteps * 2 - 2, rect.top);

            pDC->MoveTo(rect.right, rect.bottom - borderWidth + drawnSunkSteps * 2 + 2);
            pDC->LineTo(rect.left + borderWidth - drawnSunkSteps * 2 - 2, rect.bottom - borderWidth + drawnSunkSteps * 2 + 2);

            // Dark color
            pDC->SelectObject(&darkPen);
            pDC->MoveTo(rect.left, rect.bottom);
            pDC->LineTo(rect.right, rect.bottom);
            pDC->LineTo(rect.right, rect.bottom - borderWidth + drawnSunkSteps * 2 + 2);

            pDC->MoveTo(rect.left + borderWidth - drawnSunkSteps * 2 - 2, rect.bottom - borderWidth + drawnSunkSteps * 2 + 2);
            pDC->LineTo(rect.left + borderWidth - drawnSunkSteps * 2 - 2, rect.top);
        }

        rect.DeflateRect(1, 1);
    }
}