#include "pch.h"
#include "IWX11TitleBarBtnBase.h"

IWX11TitleBarBtnBase::IWX11TitleBarBtnBase(COLORREF backgroundColor, COLORREF iconColor) : IWTitleBarBtn(backgroundColor)
{
    this->iconColor = iconColor;
}

void IWX11TitleBarBtnBase::DrawSymbol(CDC* pDC, CRect rect, bool focused)
{
    // Draw border (optional: use a different color on hover)
    CPen pen(PS_SOLID, 1, this->iconColor); // Black pen for the border
    CPen* oldPen = pDC->SelectObject(&pen);
    pDC->SelectStockObject(HOLLOW_BRUSH); // No fill for the rectangle
    pDC->Rectangle(&rect);
    pDC->SelectObject(oldPen);

    this->DrawIcon(pDC, rect, focused);
}
