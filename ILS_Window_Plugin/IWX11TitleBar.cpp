#include "pch.h"
#include "IWX11TitleBar.h"

#include "IWX11CloseBtn.h"
#include "IWX11MenuBtn.h"
#include "IWX11ResizeBtn.h"

IWX11TitleBar::IWX11TitleBar(std::string title, COLORREF backgroundColor, COLORREF textColor, IWTitleBarEventListener* listener)
    : IWTitleBar(title, backgroundColor, 14, listener)
{
    this->closeButton = new IWX11CloseBtn();
    this->menuButton = new IWX11MenuBtn();
    this->resizeButton = new IWX11ResizeBtn();
}

void IWX11TitleBar::PositionButtons(const CRect& rect)
{
    const int margin = 6;
    const int btnHeight = rect.Height() - 9;
    const int btnWidth = btnHeight * 1.2;

    // V center the buttons
    const int top = (rect.Height() - btnHeight) / 2;
    const int bottom = top + btnHeight;

    int right = rect.right - 3;
    int left = right - btnWidth;

    // Position the resize button
    CRect resizeButtonRect(left, top, right, bottom);
    if (resizeButton->GetSafeHwnd())
        resizeButton->MoveWindow(resizeButtonRect);

    right = left - margin;
    left = right - btnWidth;

    // Position the menu button
    CRect menuButtonRect(left, top, right, bottom);
    if (menuButton->GetSafeHwnd())
        menuButton->MoveWindow(menuButtonRect);

    right = left - margin;
    left = right - btnWidth;

    // Position the close button
    CRect closeButtonRect(left, top, right, bottom);
    if (closeButton->GetSafeHwnd())
        closeButton->MoveWindow(closeButtonRect);
}

void IWX11TitleBar::DrawTitle(CDC* pdc, CRect rect)
{
    CRect textArea = rect;
    textArea.left += 5;
    auto oldFont = pdc->SelectObject(this->font);
    pdc->SetTextColor(this->textColor);
    pdc->SetBkMode(TRANSPARENT);
    pdc->DrawText(_T(this->text.c_str()), -1, textArea, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    pdc->SelectObject(oldFont);
}
