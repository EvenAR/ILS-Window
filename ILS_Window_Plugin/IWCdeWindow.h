#pragma once
#include "IWWindow.h"
class IWCdeWindow :
    public IWWindow
{

public:
    IWCdeWindow(IWApproachDefinition selectedApproach, IWStyling styling);

private:
    void DrawBorder(CDC* pdc, CRect rect) override;
    void DrawMenuItem(CDC* pdc, CRect bounds, CString text, bool isHovered, bool isChecked) override;
    virtual int GetEdgeCursorPosition(CPoint point) override;

    COLORREF AdjustColorBrightness(COLORREF color, double factor);

    const COLORREF lightColor;
    const COLORREF darkColor;
    CFont menuFont;
};
