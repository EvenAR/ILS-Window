#pragma once
#include "IWTitleBarBtn.h"

class IWX11CloseBtn : public IWTitleBarBtn
{
    void DrawSymbol(CDC* pDC, CRect rect) override;

};

