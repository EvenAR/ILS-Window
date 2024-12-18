#pragma once
#include <afxwin.h>
#include <string>
#include "ILSWindowCloseButton.h"
#include "ILSWindowMenuButton.h"
#include "ILSWindowResizeButton.h"

#define IDC_CLOSE_BUTTON 1001
#define IDC_MENU_BUTTON 1002
#define IDC_RESIZE_BUTTON 1003

class IParWindowTitleBarEventListener {
public:
    virtual void OnResizeStart() = 0;
    virtual void OnCloseButtonClicked() = 0;
};

class ParWindowTitleBar : public CStatic {
    DECLARE_DYNAMIC(ParWindowTitleBar)

    public:
        ParWindowTitleBar(
            const std::string& title, 
            COLORREF backgroundColor,
            COLORREF textColor, 
            COLORREF outerFrameColor, 
            IParWindowTitleBarEventListener* listener
        );
        virtual ~ParWindowTitleBar() {}

        // Initialize the top bar
        BOOL CreateTopBar(CWnd* pParentWnd, const CRect& rect, UINT nID);

    private: 
        ILSWindowCloseButton closeButton;
        ILSWindowMenuButton menuButton;
        ILSWindowResizeButton resizeButton;
        CFont euroScopeFont;

        COLORREF backgroundColor;
        COLORREF textColor;
        CPen outerFramePen;
        std::string text;
        IParWindowTitleBarEventListener* eventListener;

        afx_msg void OnPaint();
        afx_msg void OnCloseButtonClicked();
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnSize(UINT nType, int cx, int cy);

        void PositionButtons(const CRect& rect);

        DECLARE_MESSAGE_MAP()
};