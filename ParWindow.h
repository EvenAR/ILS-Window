#pragma once
#include <afxwin.h>
#include "ParDataTypes.h"

#include "WindowTitleBar.h"

#define IDC_CLOSE_BUTTON 1001
#define IDC_TOPBAR       1002
#define WM_UPDATE_DATA (WM_USER + 1)

#define TARGET_RADIUS 6
#define PI 3.14159265359
#define FT_PER_NM 6076.11549

class ParWindow;

class IParWindowEventListener {
public:
    virtual void OnWindowClosed(ParWindow* window) = 0;
};

class ParWindow : public CWnd {
    protected:
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg  void OnPaint();
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL PreCreateWindow(CREATESTRUCT& cs);
        afx_msg LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);

        void OnDestroy();

        CRect GetClientRectBelowTitleBar();
        bool leftToRight = false;

        DECLARE_MESSAGE_MAP()

    public:
        ParWindow();
        BOOL CreateCanvas(CWnd* pParentWnd, const RECT& rect, UINT nID);
        void SetListener(IParWindowEventListener* listener);

    private:
        WindowTitleBar m_CustomTopBar;
        CButton m_CloseButton;
        ParData m_latestParData;
        void DrawDiamond(CPoint pt, int size, CDC& dc);

        COLORREF windowBackground;
        CPen glideSlopePen;
        CBrush localizerBrush;
        CPen radarTargetPen;
        IParWindowEventListener* m_listener = nullptr;
};