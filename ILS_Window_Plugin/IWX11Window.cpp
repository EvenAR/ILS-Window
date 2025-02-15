#include "pch.h"
#include "IWX11Window.h"
#include "IWX11TitleBar.h"

IWX11Window::IWX11Window(IWApproachDefinition selectedApproach, IWStyling styling)
    : IWWindow(selectedApproach, styling, 26, 3, 1)
{
    this->titleBar = new IWX11TitleBar(windowBorderColor, styling.windowFrameTextColor, this);
    this->menuBgColor = styling.windowFrameColor;
    this->menuTextColor = styling.windowFrameTextColor;
    this->extraMenuItemWidth = 20;
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
    // The X11 theme does not support resizing using the window edge
    return 0;
}

void IWX11Window::DrawMenuItem(CDC* pdc, CRect bounds, CString text, bool isHovered, bool isChecked)
{
    // Invert colors when hovered
    COLORREF bgColor = isHovered ? this->menuTextColor : this->menuBgColor;
    COLORREF textColor = isHovered ? this->menuBgColor : this->menuTextColor;

    std::string fullText = isChecked ? "¤ " : "  ";
    fullText += text;

    CBrush brush(bgColor);
    pdc->FillRect(&bounds, &brush);

    // Draw text
    pdc->SetTextColor(textColor);
    pdc->SetBkMode(TRANSPARENT);

    CRect textArea = bounds;
    textArea.left += 10;

    CFont* oldFont = pdc->SelectObject(&mainFont);
    pdc->DrawText(fullText.c_str(), &textArea, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    pdc->SelectObject(&oldFont);
}
