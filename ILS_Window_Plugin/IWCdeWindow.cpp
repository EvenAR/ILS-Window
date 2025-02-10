#include "pch.h"
#include "IWCdeWindow.h"
#include "RenderUtils.h"
#include "IWCdeTitleBar.h"

IWCdeWindow::IWCdeWindow(IWApproachDefinition selectedApproach, IWStyling styling) 
    : IWWindow(selectedApproach, styling, 24, 5, 1)
    , lightColor(AdjustColorBrightness(styling.windowFrameColor, 1.4))
    , darkColor(AdjustColorBrightness(styling.windowFrameColor, 0.4))
{
    this->titleBar = new IWCdeTitleBar(windowBorderColor, styling.windowFrameTextColor, lightColor, darkColor, this);
    this->extraMenuItemWidth = 20;
}

int IWCdeWindow::GetEdgeCursorPosition(CPoint point)
{
    CRect clientRect;
    GetClientRect(&clientRect);

    bool topLeftCorner = 
        point.x < clientRect.left + TITLE_BAR_HEIGHT && point.y < clientRect.top + WINDOW_BORDER_THICKNESS || 
        point.x < clientRect.left + WINDOW_BORDER_THICKNESS && point.y < clientRect.top + TITLE_BAR_HEIGHT;

    bool topRightCorner =
        point.x > clientRect.right - TITLE_BAR_HEIGHT && point.y < clientRect.top + WINDOW_BORDER_THICKNESS ||
        point.x > clientRect.right - WINDOW_BORDER_THICKNESS && point.y < clientRect.top + TITLE_BAR_HEIGHT;

    bool bottomLeftCorner =
        point.x < clientRect.left + TITLE_BAR_HEIGHT && point.y > clientRect.bottom - WINDOW_BORDER_THICKNESS ||
        point.x < clientRect.left + WINDOW_BORDER_THICKNESS && point.y > clientRect.bottom - TITLE_BAR_HEIGHT;

    bool bottomRightCorner =
        point.x > clientRect.right - TITLE_BAR_HEIGHT && point.y > clientRect.bottom - WINDOW_BORDER_THICKNESS ||
        point.x > clientRect.right - WINDOW_BORDER_THICKNESS && point.y > clientRect.bottom - TITLE_BAR_HEIGHT;

    bool topEdge = point.y < clientRect.top + WINDOW_BORDER_THICKNESS && point.y > clientRect.top;
    bool bottomEdge = point.y > clientRect.bottom - WINDOW_BORDER_THICKNESS && point.y < clientRect.bottom;
    bool leftEdge = point.x < clientRect.left + WINDOW_BORDER_THICKNESS && point.x > clientRect.left;
    bool rightEdge = point.x > clientRect.right - WINDOW_BORDER_THICKNESS && point.x < clientRect.right;

    if (topLeftCorner)
        return HTTOPLEFT;

    if (topRightCorner)
        return HTTOPRIGHT;

    if (bottomLeftCorner)
        return HTBOTTOMLEFT;

    if (bottomRightCorner)
        return HTBOTTOMRIGHT;

    if (topEdge)
        return HTTOP;   

    if (leftEdge)
        return HTLEFT;

    if (rightEdge)
        return HTRIGHT;

    if (bottomEdge)
        return HTBOTTOM;

    return HTNOWHERE;
}

void IWCdeWindow::DrawBorder(CDC* pdc, CRect rect)
{
    int border3dSteps = 1; // Width of each 3D effect edge

    // Edges

    CRect leftBorderRect = rect;
    leftBorderRect.top = leftBorderRect.top + TITLE_BAR_HEIGHT;
    leftBorderRect.left = leftBorderRect.left + 1;
    leftBorderRect.right = leftBorderRect.left + WINDOW_BORDER_THICKNESS - 1;
    leftBorderRect.bottom = leftBorderRect.bottom - TITLE_BAR_HEIGHT;
    DrawThick3dRect(pdc, leftBorderRect, border3dSteps, lightColor, darkColor);

    CRect bottomBorderRect = rect;
    bottomBorderRect.top = bottomBorderRect.bottom - WINDOW_BORDER_THICKNESS;
    bottomBorderRect.left = bottomBorderRect.left + TITLE_BAR_HEIGHT;
    bottomBorderRect.right = bottomBorderRect.right - TITLE_BAR_HEIGHT;
    bottomBorderRect.bottom = bottomBorderRect.bottom - 1;
    DrawThick3dRect(pdc, bottomBorderRect, border3dSteps, lightColor, darkColor);

    CRect rightBorderRect = rect;
    rightBorderRect.top = rightBorderRect.top + TITLE_BAR_HEIGHT;
    rightBorderRect.left = rightBorderRect.right - WINDOW_BORDER_THICKNESS;
    rightBorderRect.right = rightBorderRect.right - 1;
    rightBorderRect.bottom = rightBorderRect.bottom - TITLE_BAR_HEIGHT;
    DrawThick3dRect(pdc, rightBorderRect, border3dSteps, lightColor, darkColor);

    CRect topBorderRect = rect;
    topBorderRect.top = topBorderRect.top + 1;
    topBorderRect.left = topBorderRect.left + TITLE_BAR_HEIGHT;
    topBorderRect.right = topBorderRect.right - TITLE_BAR_HEIGHT;
    topBorderRect.bottom = topBorderRect.top + WINDOW_BORDER_THICKNESS - 1;
    DrawThick3dRect(pdc, topBorderRect, border3dSteps, lightColor, darkColor);

    // Corners

    CRect topLeftCornerRect = rect;
    topLeftCornerRect.top = topLeftCornerRect.top + 1;
    topLeftCornerRect.left = topLeftCornerRect.left + 1;
    topLeftCornerRect.right = topLeftCornerRect.left + TITLE_BAR_HEIGHT - 2;
    topLeftCornerRect.bottom = topLeftCornerRect.top + TITLE_BAR_HEIGHT - 2;
    DrawThick3dCorner(pdc, topLeftCornerRect, WINDOW_BORDER_THICKNESS, border3dSteps, lightColor, darkColor, true, true);

    CRect topRightCornerRect = rect;
    topRightCornerRect.top = topRightCornerRect.top + 1;
    topRightCornerRect.left = topRightCornerRect.right - TITLE_BAR_HEIGHT;
    topRightCornerRect.right = topRightCornerRect.right - 2;
    topRightCornerRect.bottom = topRightCornerRect.top + TITLE_BAR_HEIGHT - 2;
    DrawThick3dCorner(pdc, topRightCornerRect, WINDOW_BORDER_THICKNESS, border3dSteps, lightColor, darkColor, true, false);

    CRect bottomRightCornerRect = rect;
    bottomRightCornerRect.top = bottomRightCornerRect.bottom - TITLE_BAR_HEIGHT;
    bottomRightCornerRect.left = bottomRightCornerRect.right - TITLE_BAR_HEIGHT;
    bottomRightCornerRect.right = bottomRightCornerRect.right - 2;
    bottomRightCornerRect.bottom = bottomRightCornerRect.bottom - 2;
    DrawThick3dCorner(pdc, bottomRightCornerRect, WINDOW_BORDER_THICKNESS, border3dSteps, lightColor, darkColor, false, false);

    CRect bottomLeftCornerRect = rect;
    bottomLeftCornerRect.top = bottomLeftCornerRect.bottom - TITLE_BAR_HEIGHT;
    bottomLeftCornerRect.left = bottomLeftCornerRect.left + 1;
    bottomLeftCornerRect.right = bottomLeftCornerRect.left + TITLE_BAR_HEIGHT - 2;
    bottomLeftCornerRect.bottom = bottomLeftCornerRect.bottom - 2;
    DrawThick3dCorner(pdc, bottomLeftCornerRect, WINDOW_BORDER_THICKNESS, border3dSteps, lightColor, darkColor, false, true);
}

COLORREF IWCdeWindow::AdjustColorBrightness(COLORREF color, double factor)
{
    GetRValue(color);

    // Adjust each component
    int red = GetRValue(color) * factor;
    int green = GetGValue(color) * factor;
    int blue = GetBValue(color) * factor;

    // Ensure the components are within the valid range
    red = max(0, min(255, red));
    green = max(0, min(255, green));
    blue = max(0, min(255, blue));

    // Combine them back into a COLORREF
    return RGB(red, green, blue);
}

void IWCdeWindow::DrawMenuItem(CDC* pdc, CRect bounds, CString text, bool isHovered, bool isChecked)
{
    COLORREF bgColor = isHovered ? RGB(130, 130, 130) : RGB(152, 152, 152);
    COLORREF textColor = RGB(255, 255, 255);  // White text

    std::string fullText = isChecked ? "¤ " : "  ";
    fullText += text;

    CBrush brush(bgColor);
    pdc->FillRect(&bounds, &brush);

    if (isHovered) {
        COLORREF darkened = AdjustColorBrightness(bgColor, 0.6);
        COLORREF lightened = AdjustColorBrightness(bgColor, 1.4);
        DrawThick3dRect(pdc, bounds, 2, darkened, lightened);
    }

    // Draw text
    pdc->SetTextColor(textColor);
    pdc->SetBkMode(TRANSPARENT);

    CRect textArea = bounds;
    textArea.left += 10;

    CFont* oldFont = pdc->SelectObject(&mainFont);
    pdc->DrawText(fullText.c_str(), &textArea, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    pdc->SelectObject(&oldFont);
}
