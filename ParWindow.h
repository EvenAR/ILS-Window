#pragma once
#include <afxwin.h>
#include "ParDataTypes.h"

#include "WindowTitleBar.h"

#define IDC_CLOSE_BUTTON 1001
#define IDC_TOPBAR       1002
#define WM_UPDATE_DATA (WM_USER + 1)

#define TARGET_RADIUS 6
#define HISTORY_TRAIL_RADIUS 4
#define PI 3.14159265359
#define FT_PER_NM 6076.11549

#define TITLE_BAR_HEIGHT 20

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
        afx_msg void OnDestroy();
        afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg BOOL OnNcActivate(BOOL bActive);
        afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
        afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

        void DrawContent(CDC& dc);
        CRect GetClientRectBelowTitleBar();
        

        DECLARE_MESSAGE_MAP()

    public:
        ParWindow(const char* title, double appSlope, double appLength, bool leftToRight, ParStyling styling);
        virtual ~ParWindow();
        void SetListener(IParWindowEventListener* listener);

    private:
        WindowTitleBar titleBar;
        ParData m_latestParData;
        void DrawDiamond(CPoint pt, int size, CDC& dc);

        COLORREF windowBackground;
        COLORREF targetLabelColor;
        CPen glideSlopePen;
        CBrush localizerBrush;
        CPen radarTargetPen;
        CPen historyTrailPen;
        IParWindowEventListener* m_listener = nullptr;

        double approachSlope;
        double approachLength;
        bool leftToRight;
};