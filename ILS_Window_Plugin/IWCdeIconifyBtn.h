#pragma once

#include "IWTitleBarBtn.h"

class IWCdeIconifyBtn : public IWTitleBarBtn
{
public:
    IWCdeIconifyBtn(COLORREF lightColor, COLORREF darkColor);

private:
    void DrawSymbol(CDC* pDC, CRect rect) override;

    COLORREF lightColor;
    COLORREF darkColor;
};

