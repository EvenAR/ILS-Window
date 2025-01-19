#pragma once
#include <afxwin.h>
#include "IWDataTypes.h"
#include "IWTitleBar.h"
#include "IWVisualization.h"
#include <set>
#include <mutex>

#define IDC_CLOSE_BUTTON 1001
#define IDC_TOPBAR       1002
#define WM_UPDATE_DATA (WM_USER + 1)

#define TITLE_BAR_HEIGHT 27
#define SIZE_SNAP_INCREMENTS 20.0
#define WINDOW_BORDER_WIDTH 4
#define WINDOW_OUTER_BORDER_WIDTH 1

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

        BOOL OnMenuOptionSelected(UINT nID);
        void OnProcedureSelected(UINT nID);

        CRect GetClientRectBelowTitleBar();
                
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
        IWVisualization ilsVisualization;
        COLORREF windowBorderColor;
        COLORREF windowOuterBorderColor;

        void CreatePopupMenu(CPoint point);

        IWApproachDefinition selectedApproach;
        std::vector<IWApproachDefinition> availableApproaches;

        IIWWndEventListener* m_listener = nullptr;

        CFont font;

        // For handling events from the title bar
        void OnResizeStart() override;
        void OnCloseButtonClicked() override;
        void OnMenuButtonClicked() override;

        // For thread safety between EuroScope and the window thread
        IWLiveData m_latestLiveData;
        mutable std::mutex approachDataMutex;
};
