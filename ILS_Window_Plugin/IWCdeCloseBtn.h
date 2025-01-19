#pragma once

#include "IWTitleBarBtn.h"

class IWCdeCloseBtn : public IWTitleBarBtn
{
public:
    IWCdeCloseBtn(COLORREF lightColor, COLORREF darkColor);

private:
    void DrawSymbol(CDC* pDC, CRect rect) override;

    COLORREF lightColor;
    COLORREF darkColor;
};

