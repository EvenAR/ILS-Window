#pragma once
#include <afxwin.h>
#include <string>

#define IDC_CLOSE_BUTTON 1001

class WindowTitleBar : public CStatic {
    DECLARE_DYNAMIC(WindowTitleBar)

    public:
        WindowTitleBar(const std::string& title, COLORREF backgroundColor, COLORREF textColor);
        virtual ~WindowTitleBar() {}

        // Initialize the top bar
        BOOL CreateTopBar(CWnd* pParentWnd, const CRect& rect, UINT nID);

    private: 
        COLORREF backgroundColor;
        COLORREF textColor;
        std::string text;
    protected:
        afx_msg void OnPaint();
        afx_msg void OnCloseButtonClicked();
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnSize(UINT nType, int cx, int cy);
        CButton m_CloseButton;

        DECLARE_MESSAGE_MAP()
};