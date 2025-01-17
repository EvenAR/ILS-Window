#pragma once
#include <afxwin.h>
#include "IWDataTypes.h"
#include "IWTitleBar.h"
#include <set>
#include <mutex>

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
    virtual void OnWindowMenuOpenNew(std::string title) = 0;
    virtual void OnWindowRectangleChanged(IWWindow* window) = 0;
};

class IWWindow : public CWnd, IWTitleBarEventListener {
    protected:
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg void OnPaint();
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg LRESULT OnExitSizeMove(WPARAM wParam, LPARAM lParam);
        afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
        afx_msg BOOL PreCreateWindow(CREATESTRUCT& cs);
        afx_msg LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);
        afx_msg void OnDestroy();
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg BOOL OnNcActivate(BOOL bActive);
        afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
        afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

        BOOL OnMenuOptionSelected(UINT nID);
        void OnProcedureSelected(UINT nID);

        void DrawContent(CDC& dc);
        CRect GetClientRectBelowTitleBar();

        afx_msg void OnTimer(UINT_PTR nIDEvent);

        
        DECLARE_MESSAGE_MAP()

    public:
        IWWindow(IWApproachDefinition selectedApproach, IWStyling styling);
        virtual ~IWWindow();
        void SetListener(IIWWndEventListener* listener);
        std::string GetActiveApproachName() const;
        void SetActiveApproach(const IWApproachDefinition& selectedApproach);
        void SetAvailableApproaches(const std::vector<IWApproachDefinition>& approaches);

    private:
        IWTitleBar titleBar;
        IWLiveData m_latestLiveData;
        void DrawDiamond(CPoint pt, int size, CDC& dc);
        bool CalculateTargetCoordinates(const IWTargetPosition& position, CPoint& ptTopView, CPoint& ptSideView);
        void UpdateDimentions();
        void CreatePopupMenu(CPoint point);

        IWApproachDefinition selectedApproach;
        std::vector<IWApproachDefinition> availableApproaches;

        double CalculateDistance(double lat1, double lon1, double lat2, double lon2);
        double CalculateBearing(double lat1, double lon1, double lat2, double lon2);
        double CalculateTemperatureCorrection(int planePressAlt, int airportPressureAlt, double surfTemp);

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

        int approachLength;
        bool leftToRight;
        bool applyTemperatureCorrection = true;

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
        void OnMenuButtonClicked() override;

        // For thread safety between EuroScope and the window thread
        mutable std::mutex approachDataMutex;
};
