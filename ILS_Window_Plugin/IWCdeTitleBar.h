#pragma once
#include <afxwin.h>
#include <string>
#include "IWTitleBar.h"

class IWTitleBarEventListener;

class IWCdeTitleBar : public IWTitleBar {
    public:
        IWCdeTitleBar(COLORREF backgroundColor, COLORREF textColor, COLORREF lightColor, COLORREF darkcolor, IWTitleBarEventListener* listener);
        virtual ~IWCdeTitleBar() {}

    private: 
        void PositionButtons(const CRect& rect) override;
        void DrawTitle(CDC* pdc, CRect rect, CString title) override;

        COLORREF backgroundColor;
        COLORREF textColor;
        COLORREF lightColor;
        COLORREF darkColor;

        CRect titleArea;
};