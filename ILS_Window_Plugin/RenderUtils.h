#pragma once

#include <afxwin.h>

inline void DrawThick3dRect(CDC* pDC, CRect rect, int steps, COLORREF lightColor, COLORREF darkColor) 
{
    for (int drawnSunkSteps = 0; drawnSunkSteps < steps; drawnSunkSteps++) {
        pDC->Draw3dRect(rect, lightColor, darkColor);
        rect.DeflateRect(1, 1);
    }
}

inline void DrawThick3dCorner(CDC* pDC, CRect rect, int borderWidth, int steps, COLORREF lightColor, COLORREF darkColor, bool isTop, bool isLeft)
{
    CPen lightPen(PS_SOLID, 1, lightColor);
    CPen darkPen(PS_SOLID, 1, darkColor);

    for (int drawnSunkSteps = 0; drawnSunkSteps < steps; drawnSunkSteps++) {
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