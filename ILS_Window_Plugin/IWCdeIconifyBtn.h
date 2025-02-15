#pragma once

#include "IWTitleBarBtn.h"

class IWCdeIconifyBtn : public IWTitleBarBtn
{
public:
    IWCdeIconifyBtn(COLORREF backgroundColor, COLORREF lightColor, COLORREF darkColor);

private:
    void DrawSymbol(CDC* pDC, CRect rect, bool focused) override;

    COLORREF lightColor;
    COLORREF darkColor;
};

