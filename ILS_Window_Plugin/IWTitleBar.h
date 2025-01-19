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
    virtual void OnMenuButtonClicked() = 0;
};

class IWTitleBar : public CStatic {
    DECLARE_DYNAMIC(IWTitleBar)

    public:
        IWTitleBar(
            std::string title,
            COLORREF backgroundColor,
            COLORREF textColor, 
            IWTitleBarEventListener* listener
        );
        virtual ~IWTitleBar() {}

        // Initialize the top bar
        BOOL CreateTopBar(CWnd* pParentWnd, const CRect& rect, UINT nID);

        void SetTitle(const std::string& title) { this->text = title; }

    private: 
        IWCloseBtn closeButton;
        IWMenuBtn menuButton;
        IWResizeBtn resizeButton;
        CFont font;

        COLORREF backgroundColor;
        COLORREF textColor;
        std::string text;
        IWTitleBarEventListener* eventListener;

        afx_msg void OnPaint();
        afx_msg void OnCloseButtonClicked();
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnSize(UINT nType, int cx, int cy);

        void PositionButtons(const CRect& rect);

        DECLARE_MESSAGE_MAP()
};