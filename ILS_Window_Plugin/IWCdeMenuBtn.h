#pragma once

#include "IWTitleBarBtn.h"

class IWCdeMenuBtn : public IWTitleBarBtn
{
public:
    IWCdeMenuBtn(COLORREF backgroundColor, COLORREF lightColor, COLORREF darkColor);

private:
    void DrawSymbol(CDC* pDC, CRect rect, bool focused) override;

    COLORREF lightColor;
    COLORREF darkColor;
};

