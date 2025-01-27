#pragma once
#include "IWTitleBarBtn.h"

class IWX11IconifyBtn : public IWTitleBarBtn
{
    using IWTitleBarBtn::IWTitleBarBtn;
    void DrawSymbol(CDC* pDC, CRect rect) override;

};

