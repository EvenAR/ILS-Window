#pragma once
#include "IWTitleBarBtn.h"

class IWX11IconifyBtn : public IWTitleBarBtn
{
    void DrawSymbol(CDC* pDC, CRect rect) override;

};

