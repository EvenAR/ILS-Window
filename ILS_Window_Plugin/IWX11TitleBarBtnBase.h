#pragma once

#include "IWTitleBarBtn.h"

class IWX11TitleBarBtnBase : public IWTitleBarBtn
{
public:
    IWX11TitleBarBtnBase(COLORREF backgroundColor, COLORREF iconColor);

private:
    void DrawSymbol(CDC* pDC, CRect rect, bool focused) override;

protected:
    virtual void DrawIcon(CDC* pdc, CRect rect, bool focused) = 0;
    COLORREF iconColor;
};

