#include "pch.h"
#include "IWCdeMenuBtn.h"
#include "RenderUtils.h"

IWCdeMenuBtn::IWCdeMenuBtn(COLORREF backgroundColor, COLORREF lightColor, COLORREF darkColor) : IWTitleBarBtn(backgroundColor)
{
    this->lightColor = lightColor;
    this->darkColor = darkColor;
}

void IWCdeMenuBtn::DrawSymbol(CDC* pdc, CRect rect)
{
    CRect barIcon = rect;
    barIcon.left = rect.left + rect.Width() * 0.25;
    barIcon.right = rect.right - rect.Width() * 0.25;
    barIcon.top = rect.top + rect.Height() / 2 - 2;
    barIcon.bottom = barIcon.top + 4;

    DrawThick3dRect(pdc, rect, 1, lightColor, darkColor);
    DrawThick3dRect(pdc, barIcon, 1, lightColor, darkColor);
}
