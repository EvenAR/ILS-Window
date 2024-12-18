#pragma once
#include <afxwin.h>
#include "ParDataTypes.h"

#include "ParWindowTitleBar.h"

#define IDC_CLOSE_BUTTON 1001
#define IDC_TOPBAR       1002
#define WM_UPDATE_DATA (WM_USER + 1)

#define APP_LINE_MARGIN_TOP 0.1
#define APP_LINE_MARGIN_SIDES 0.1
#define APP_LINE_MARGIN_BOTTOM 0.3
#define LABEL_OFFSET 15
#define TARGET_RADIUS 5
#define HISTORY_TRAIL_RADIUS 5
#define PI 3.14159265359
#define FT_PER_NM 6076.11549

#define TITLE_BAR_HEIGHT 27

class ParWindow;

class IParWindowEventListener {
public:
    virtual void OnWindowClosed(ParWindow* window) = 0;
};

class ParWindow : public CWnd, IParWindowTitleBarEventListener {
    protected:
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg  void OnPaint();
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL PreCreateWindow(CREATESTRUCT& cs);
        afx_msg LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);
        afx_msg void OnDestroy();
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg BOOL OnNcActivate(BOOL bActive);
        afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
        afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

        void DrawContent(CDC& dc);
        CRect GetClientRectBelowTitleBar();

        afx_msg void OnTimer(UINT_PTR nIDEvent);
        

        DECLARE_MESSAGE_MAP()

    public:
        ParWindow(const char* title, double appSlope, double appLength, bool leftToRight, float maxOffsetLeft, float maxOffsetRight, ParStyling styling);
        virtual ~ParWindow();
        void SetListener(IParWindowEventListener* listener);

    private:
        ParWindowTitleBar titleBar;
        ParData m_latestParData;
        void DrawDiamond(CPoint pt, int size, CDC& dc);

        COLORREF rangeStatusTextColor;
        COLORREF windowBackground;
        COLORREF targetLabelColor;
        CPen glideSlopePen;
        CBrush localizerBrush;
        CPen radarTargetPen;
        CPen historyTrailPen;
        CPen windowBorderPen;
        CPen windowOuterBorderPen;
        IParWindowEventListener* m_listener = nullptr;

        double approachSlope;
        int approachLength;
        bool leftToRight;
        float maxOffsetLeft;
        float maxOffsetRight;

        bool showZoomMessage = false;
        UINT_PTR zoomMessageTimerId = 1;

        CFont euroScopeFont;

        // For handling events from the title bar
        void OnResizeStart() override;
        void OnCloseButtonClicked() override;
};