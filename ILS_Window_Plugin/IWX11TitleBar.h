#pragma once
#include "IWTitleBar.h"

class IWX11TitleBar :  public IWTitleBar
{
public:
    IWX11TitleBar(std::string title, COLORREF backgroundColor, COLORREF textColor, IWTitleBarEventListener* listener);
    virtual ~IWX11TitleBar() {}

private:
    void PositionButtons(const CRect& rect) override;
    void DrawTitle(CDC* pdc, CRect rect) override;

    COLORREF textColor;
};

