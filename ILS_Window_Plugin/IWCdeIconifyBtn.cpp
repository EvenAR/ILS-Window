#include "pch.h"
#include "IWCdeIconifyBtn.h"
#include "RenderUtils.h"

IWCdeIconifyBtn::IWCdeIconifyBtn(COLORREF backgroundColor, COLORREF lightColor, COLORREF darkColor) : IWTitleBarBtn(backgroundColor)
{
    this->lightColor = lightColor;
    this->darkColor = darkColor;
}

void IWCdeIconifyBtn::DrawSymbol(CDC* pdc, CRect rect, bool focused)
{
    CRect buttonFrame = rect;
    buttonFrame.left = rect.right - rect.Height();

    CRect icon = buttonFrame;
    icon.left = buttonFrame.left + buttonFrame.Width() * 0.4;
    icon.right = icon.right - buttonFrame.Width() * 0.4;
    icon.top = buttonFrame.top + buttonFrame.Width() * 0.4;
    icon.bottom = buttonFrame.bottom - buttonFrame.Width() * 0.4;

    if (focused) {
        DrawThick3dRect(pdc, rect, 1, darkColor, lightColor);
    }
    else {
        DrawThick3dRect(pdc, rect, 1, lightColor, darkColor);
    }

    DrawThick3dRect(pdc, icon, 1, lightColor, darkColor);
}