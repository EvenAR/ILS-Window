#pragma once
#include <afxwin.h>
#include <string>
#include "IWCloseBtn.h"
#include "IWMenuBtn.h"
#include "IWResizeBtn.h"

#define IDC_CLOSE_BUTTON 1001
#define IDC_MENU_BUTTON 1002
#define IDC_RESIZE_BUTTON 1003

class IWTitleBarEventListener {
public:
    virtual void OnResizeStart() = 0;
    virtual void OnCloseButtonClicked() = 0;
};

class IWTitleBar : public CStatic {
    DECLARE_DYNAMIC(IWTitleBar)

    public:
        IWTitleBar(
            const std::string& title, 
            COLORREF backgroundColor,
            COLORREF textColor, 
            COLORREF outerFrameColor, 
            IWTitleBarEventListener* listener
        );
        virtual ~IWTitleBar() {}

        // Initialize the top bar
        BOOL CreateTopBar(CWnd* pParentWnd, const CRect& rect, UINT nID);

    private: 
        IWCloseBtn closeButton;
        IWMenuBtn menuButton;
        IWResizeBtn resizeButton;
        CFont euroScopeFont;

        COLORREF backgroundColor;
        COLORREF textColor;
        CPen outerFramePen;
        std::string text;
        IWTitleBarEventListener* eventListener;

        afx_msg void OnPaint();
        afx_msg void OnCloseButtonClicked();
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnSize(UINT nType, int cx, int cy);

        void PositionButtons(const CRect& rect);

        DECLARE_MESSAGE_MAP()
};