#pragma once
#include "IWWindow.h"
class IWX11Window :
    public IWWindow
{
public:
    IWX11Window(IWApproachDefinition selectedApproach, IWStyling styling);

private:
    void DrawBorder(CDC* pdc, CRect windowRect) override;
    int GetEdgeCursorPosition(CPoint point) override;
};

