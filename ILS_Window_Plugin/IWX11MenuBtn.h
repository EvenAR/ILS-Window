#pragma once
#include "IWTitleBarBtn.h"

class IWX11MenuBtn : public IWTitleBarBtn
{
    using IWTitleBarBtn::IWTitleBarBtn;
    void DrawSymbol(CDC* pDC, CRect rect) override;
};

