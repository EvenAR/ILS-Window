#pragma once
#include "IWTitleBarBtn.h"

class IWCloseBtn : public IWTitleBarBtn
{
    void DrawSymbol(CDC* pDC, CRect rect) override;

};

