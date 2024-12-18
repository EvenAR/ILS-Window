#pragma once
#include "ILSWindowTitleBarButton.h"
class ILSWindowCloseButton :
    public ILSWindowTitleBarButton
{
    void DrawSymbol(CDC* pDC, CRect rect) override;

};

