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
    void DrawMenuItem(CDC* pdc, CRect bounds, CString text, bool isHovered, bool isChecked) override;

    COLORREF menuBgColor;
    COLORREF menuTextColor;
    CFont menuFont;
};

