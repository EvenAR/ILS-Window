#pragma once
#include "ILSWindowTitleBarButton.h"
class ILSWindowMenuButton :
    public ILSWindowTitleBarButton
{
    void DrawSymbol(CDC* pDC, CRect rect) override;
};

