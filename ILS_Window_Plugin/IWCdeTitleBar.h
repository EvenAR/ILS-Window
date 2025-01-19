#pragma once
#include <afxwin.h>
#include <string>
#include "IWTitleBar.h"

class IWTitleBarEventListener;

class IWCdeTitleBar : public IWTitleBar {
    public:
        IWCdeTitleBar(std::string title, COLORREF backgroundColor, COLORREF textColor, COLORREF lightColor, COLORREF darkcolor, IWTitleBarEventListener* listener);
        virtual ~IWCdeTitleBar() {}

    private: 
        void PositionButtons(const CRect& rect) override;
        void DrawTitle(CDC* pdc, CRect rect) override;

        COLORREF backgroundColor;
        COLORREF textColor;
        COLORREF lightColor;
        COLORREF darkColor;

        CRect titleArea;
};