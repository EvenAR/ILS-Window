#pragma once
#include "IWTitleBarBtn.h"

class IWMenuBtn : public IWTitleBarBtn
{
    void DrawSymbol(CDC* pDC, CRect rect) override;
};

