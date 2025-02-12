#include "pch.h"
#include "IWX11TitleBar.h"

#include "IWX11IconifyBtn.h"
#include "IWX11MenuBtn.h"
#include "IWX11ResizeBtn.h"

IWX11TitleBar::IWX11TitleBar(COLORREF backgroundColor, COLORREF textColor, IWTitleBarEventListener* listener)
    : IWTitleBar(backgroundColor, 14, listener)
{
    this->iconifyButton = new IWX11IconifyBtn(backgroundColor, textColor);
    this->menuButton = new IWX11MenuBtn(backgroundColor, textColor);
    this->resizeButton = new IWX11ResizeBtn(backgroundColor, textColor);
    this->textColor = textColor;
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

    // Position the iconify button
    CRect iconifyButtonRect(left, top, right, bottom);
    if (iconifyButton->GetSafeHwnd())
        iconifyButton->MoveWindow(iconifyButtonRect);
}

void IWX11TitleBar::DrawTitle(CDC* pdc, CRect rect, CString title)
{
    CRect textArea = rect;
    textArea.left += 5;
    auto oldFont = pdc->SelectObject(this->mainFont);
    pdc->SetTextColor(this->textColor);
    pdc->SetBkMode(TRANSPARENT);
    pdc->DrawText(_T(title), -1, textArea, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    pdc->SelectObject(oldFont);
}
