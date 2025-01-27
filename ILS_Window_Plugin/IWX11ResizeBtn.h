#pragma once
#include "IWTitleBarBtn.h"

class IWX11ResizeBtn : public IWTitleBarBtn
{
    using IWTitleBarBtn::IWTitleBarBtn;
    void DrawSymbol(CDC* pDC, CRect rect) override;
};
