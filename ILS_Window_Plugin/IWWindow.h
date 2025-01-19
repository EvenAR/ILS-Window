#pragma once
#include <afxwin.h>
#include "IWDataTypes.h"
#include "IWVisualization.h"
#include "IWTitleBar.h"
#include <set>
#include <mutex>

#define IDC_CLOSE_BUTTON 1001
#define IDC_TOPBAR       1002
#define WM_UPDATE_DATA (WM_USER + 1)

#define SIZE_SNAP_INCREMENTS 20.0

class IWWindow;


class IIWWndEventListener {
public:
    virtual void OnWindowClosed(IWWindow* window) = 0;
    virtual void OnWindowMenuOpenNew(std::string title) = 0;
    virtual void OnWindowRectangleChanged(IWWindow* window) = 0;
};

class IWWindow : public CWnd, public IWTitleBarEventListener {
    DECLARE_MESSAGE_MAP()

    public:
        IWWindow(IWApproachDefinition selectedApproach, IWStyling styling, int titleBarHeight, int windowBorderThickness, int windowOuterBorderThickness);
        virtual ~IWWindow();
        void SetListener(IIWWndEventListener* listener);
        std::string GetActiveApproachName() const;
        void SetActiveApproach(const IWApproachDefinition& selectedApproach);
        void SetAvailableApproaches(const std::vector<IWApproachDefinition>& approaches);

    protected:
        virtual void DrawBorder(CDC* pdc, CRect windowRect) = 0;
        virtual int GetEdgeCursorPosition(CPoint point) = 0;
        
        const int TITLE_BAR_HEIGHT;
        const int WINDOW_BORDER_THICKNESS;
        const int WINDOW_OUTER_BORDER_WIDTH;
        const COLORREF textColor;
        const COLORREF windowBorderColor;
        const COLORREF windowOuterBorderColor;

        IWTitleBar* titleBar;


    private:

        IWVisualization ilsVisualization;
        CFont font;

        void CreatePopupMenu(CPoint point);

        IWApproachDefinition selectedApproach;
        std::vector<IWApproachDefinition> availableApproaches;

        IIWWndEventListener* m_listener = nullptr;

        // For handling events from the title bar
        void OnResizeStart() override;
        void OnCloseButtonClicked() override;
        void OnMenuButtonClicked() override;

        // For thread safety between EuroScope and the window thread
        IWLiveData m_latestLiveData;
        mutable std::mutex approachDataMutex;

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
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

        BOOL OnMenuOptionSelected(UINT nID);
        void OnProcedureSelected(UINT nID);
        CRect GetClientRectBelowTitleBar();
};
