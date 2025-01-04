#pragma once
#include <afxwin.h>
#include "IWDataTypes.h"
#include "IWTitleBar.h"
#include <set>

#define IDC_CLOSE_BUTTON 1001
#define IDC_TOPBAR       1002
#define WM_UPDATE_DATA (WM_USER + 1)

#define APP_LINE_MARGIN_TOP 0.08
#define APP_LINE_MARGIN_SIDES 0.08
#define APP_LINE_MARGIN_BOTTOM 0.35
#define LABEL_OFFSET 15
#define TARGET_RADIUS 5
#define HISTORY_TRAIL_RADIUS 5
#define PI 3.14159265359
#define FT_PER_NM 6076.11549

#define TITLE_BAR_HEIGHT 27
#define SIZE_SNAP_INCREMENTS 20.0

#define EARTH_RADIUS_NM 3440.065
#define PI 3.14159265359

class IWWindow;

class IIWWndEventListener {
public:
    virtual void OnWindowClosed(IWWindow* window) = 0;
};

class IWWindow : public CWnd, IWTitleBarEventListener {
    protected:
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg  void OnPaint();
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
        afx_msg BOOL PreCreateWindow(CREATESTRUCT& cs);
        afx_msg LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);
        afx_msg void OnDestroy();
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg BOOL OnNcActivate(BOOL bActive);
        afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
        afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

        void DrawContent(CDC& dc);
        CRect GetClientRectBelowTitleBar();

        afx_msg void OnTimer(UINT_PTR nIDEvent);

        
        DECLARE_MESSAGE_MAP()

    public:
        IWWindow(IWApproachDefinition approachData, IWStyling styling);
        virtual ~IWWindow();
        void SetListener(IIWWndEventListener* listener);
        std::string GetSelectedApproach(IWApproachDefinition& approach);

    private:
        IWTitleBar titleBar;
        IWLiveData m_latestLiveData;
        void DrawDiamond(CPoint pt, int size, CDC& dc);
        bool CalculateTargetCoordinates(const IWTargetPosition& position, CPoint& ptTopView, CPoint& ptSideView);
        void UpdateDimentions();

        double CalculateDistance(double lat1, double lon1, double lat2, double lon2);
        double CalculateBearing(double lat1, double lon1, double lat2, double lon2);

        COLORREF rangeStatusTextColor;
        COLORREF windowBackground;
        COLORREF targetLabelColor;
        CPen glideSlopePen;
        CBrush localizerBrush;
        CPen radarTargetPen;
        CPen historyTrailPen;
        CPen windowBorderPen;
        CPen windowOuterBorderPen;
        IIWWndEventListener* m_listener = nullptr;
        IWTagMode tagMode;
        bool showTagsByDefault;

        std::set<std::string> clickedTargets;

        IWApproachDefinition approachData;
        int approachLength;
        bool leftToRight;

        // Dimentions
        float approachHeightFt;
        double pixelsPerFt;
        double pixelsPerNauticalMile;
        CPoint glidePathTop;
        CPoint glidePathBottom;

        bool showZoomMessage = false;
        UINT_PTR zoomMessageTimerId = 1;

        CFont euroScopeFont;

        // For handling events from the title bar
        void OnResizeStart() override;
        void OnCloseButtonClicked() override;
};