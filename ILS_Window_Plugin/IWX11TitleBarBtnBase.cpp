#include "pch.h"
#include "IWX11TitleBarBtnBase.h"

IWX11TitleBarBtnBase::IWX11TitleBarBtnBase(COLORREF backgroundColor, COLORREF iconColor) : IWTitleBarBtn(backgroundColor)
{
    backgroundDefaultBrush.CreateSolidBrush(backgroundColor);
    backgroundFocusBrush.CreateSolidBrush(iconColor);

    iconDefaultPen.CreatePen(PS_SOLID, 1, iconColor);
    iconFocusPen.CreatePen(PS_SOLID, 1, backgroundColor);
    
    iconDefaultBrush.CreateSolidBrush(iconColor);
    iconFocusBrush.CreateSolidBrush(backgroundColor);
}

void IWX11TitleBarBtnBase::DrawSymbol(CDC* pDC, CRect rect, bool focused)
{
    CBrush* oldBrush = NULL;
    CPen* oldPen = NULL;

    if (focused) {
        oldBrush = pDC->SelectObject(&backgroundFocusBrush);
        oldPen = pDC->SelectObject(&iconDefaultPen);
    }
    else {
        oldPen = pDC->SelectObject(&iconDefaultPen);
        pDC->SelectStockObject(HOLLOW_BRUSH); // No fill for the rectangle
    }

    pDC->Rectangle(&rect);

    pDC->SelectObject(oldBrush);
    pDC->SelectObject(oldPen);

    this->DrawIcon(pDC, rect, focused);
}
