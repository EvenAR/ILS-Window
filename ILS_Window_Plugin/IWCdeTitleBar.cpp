#include "pch.h"
#include "IWCdeTitleBar.h"
#include "RenderUtils.h"
#include "IWCdeIconifyBtn.h"
#include "IWCdeMenuBtn.h"
#include "IWX11ResizeBtn.h"

IWCdeTitleBar::IWCdeTitleBar(std::string title, COLORREF backgroundColor, COLORREF textColor, COLORREF lightColor, COLORREF darkcolor, IWTitleBarEventListener* listener) :
    IWTitleBar(title, backgroundColor, 13, listener)
{
    this->lightColor = lightColor;
    this->darkColor = darkcolor;
    this->textColor = textColor;

    this->iconifyButton = new IWCdeIconifyBtn(backgroundColor, lightColor, darkColor);
    this->menuButton = new IWCdeMenuBtn(backgroundColor, lightColor, darkColor);
    this->resizeButton = new IWX11ResizeBtn(backgroundColor);
}

void IWCdeTitleBar::PositionButtons(const CRect& rect)
{
    CRect menuButtonRect(rect.left, rect.top, rect.left + rect.Height(), rect.bottom);
    if (menuButton->GetSafeHwnd())
        menuButton->MoveWindow(menuButtonRect);

    CRect iconifyButtonRect(rect.right - rect.Height(), rect.top, rect.right, rect.bottom);
    if (iconifyButton->GetSafeHwnd())
        iconifyButton->MoveWindow(iconifyButtonRect);

    titleArea = rect;
    titleArea.left = menuButtonRect.right;
    titleArea.right = iconifyButtonRect.left;
}

void IWCdeTitleBar::DrawTitle(CDC* pdc, CRect rect)
{
    Draw3dRect(pdc, titleArea, 1, lightColor, darkColor);

    auto oldFont = pdc->SelectObject(this->font);
    pdc->SetTextColor(this->textColor);
    pdc->SetBkMode(TRANSPARENT);
    pdc->DrawText(_T(this->text.c_str()), -1, titleArea, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    pdc->SelectObject(oldFont);
}
