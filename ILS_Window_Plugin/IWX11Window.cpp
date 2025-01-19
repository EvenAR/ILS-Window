#include "pch.h"
#include "IWX11Window.h"
#include "IWX11TitleBar.h"

IWX11Window::IWX11Window(IWApproachDefinition selectedApproach, IWStyling styling)
    : IWWindow(selectedApproach, styling, 26, 3, 1)
{
    this->titleBar = new IWX11TitleBar(selectedApproach.title, windowBorderColor, textColor, this);
}

void IWX11Window::DrawBorder(CDC* pdc, CRect windowRect)
{
    // Draw the border
    CPen pen(PS_SOLID, 1, RGB(0, 0, 0)); // Black pen for the border
    CPen* oldPen = pdc->SelectObject(&pen);
    pdc->SelectStockObject(NULL_BRUSH); // No fill for the rectangle
    pdc->Rectangle(&windowRect);
    pdc->SelectObject(oldPen);
}

int IWX11Window::GetEdgeCursorPosition(CPoint point)
{
    return 0;
}
