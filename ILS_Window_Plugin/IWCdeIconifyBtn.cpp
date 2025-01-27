#include "pch.h"
#include "IWCdeIconifyBtn.h"
#include "RenderUtils.h"

IWCdeIconifyBtn::IWCdeIconifyBtn(COLORREF lightColor, COLORREF darkColor)
{
    this->lightColor = lightColor;
    this->darkColor = darkColor;
}

void IWCdeIconifyBtn::DrawSymbol(CDC* pdc, CRect rect)
{
    CRect buttonFrame = rect;
    buttonFrame.left = rect.right - rect.Height();

    CRect icon = buttonFrame;
    icon.left = buttonFrame.left + buttonFrame.Width() * 0.4;
    icon.right = icon.right - buttonFrame.Width() * 0.4;
    icon.top = buttonFrame.top + buttonFrame.Width() * 0.4;
    icon.bottom = buttonFrame.bottom - buttonFrame.Width() * 0.4;

    Draw3dRect(pdc, buttonFrame, 1, lightColor, darkColor);
    Draw3dRect(pdc, icon, 1, lightColor, darkColor);
}