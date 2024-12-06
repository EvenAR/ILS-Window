#pragma once
#include <afxwin.h>

#include "WindowTitleBar.h"

#define IDC_CLOSE_BUTTON 1001
#define IDC_TOPBAR       1002
#define WM_UPDATE_DATA (WM_USER + 1)

class ParWindow : public CWnd {
    protected:
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg void OnPaint();
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL PreCreateWindow(CREATESTRUCT& cs);
        afx_msg LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);

        DECLARE_MESSAGE_MAP()

    public:
        BOOL CreateCanvas(CWnd* pParentWnd, const RECT& rect, UINT nID);

    private:
        WindowTitleBar m_CustomTopBar;
        CButton m_CloseButton;

};