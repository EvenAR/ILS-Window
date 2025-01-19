#pragma once

#include "IWTitleBarBtn.h"

class IWCdeMenuBtn : public IWTitleBarBtn
{
public:
    IWCdeMenuBtn(COLORREF lightColor, COLORREF darkColor);

private:
    void DrawSymbol(CDC* pDC, CRect rect) override;

    COLORREF lightColor;
    COLORREF darkColor;
};

