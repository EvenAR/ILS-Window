#include "pch.h"
#include "IWCdeCloseBtn.h"
#include "RenderUtils.h"

IWCdeCloseBtn::IWCdeCloseBtn(COLORREF lightColor, COLORREF darkColor)
{
    this->lightColor = lightColor;
    this->darkColor = darkColor;
}

void IWCdeCloseBtn::DrawSymbol(CDC* pdc, CRect rect)
{
    CRect menuButton = rect;
    menuButton.left = rect.right - rect.Height();

    CRect closeIcon = menuButton;
    closeIcon.left = menuButton.left + menuButton.Width() * 0.4;
    closeIcon.right = closeIcon.right - menuButton.Width() * 0.4;
    closeIcon.top = menuButton.top + menuButton.Width() * 0.4;
    closeIcon.bottom = menuButton.bottom - menuButton.Width() * 0.4;

    Draw3dRect(pdc, menuButton, 1, lightColor, darkColor);
    Draw3dRect(pdc, closeIcon, 1, lightColor, darkColor);
}