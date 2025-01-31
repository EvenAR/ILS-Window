#pragma once
#include "IWWindow.h"
class IWCdeWindow :
    public IWWindow
{

public:
    IWCdeWindow(IWApproachDefinition selectedApproach, IWStyling styling);

private:
    void DrawBorder(CDC* pdc, CRect rect) override;
    COLORREF AdjustColorBrightness(RGB color, double factor);
    virtual int GetEdgeCursorPosition(CPoint point) override;

    const COLORREF lightColor;
    const COLORREF darkColor;
};
