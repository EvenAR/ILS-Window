#pragma once
#include <afxwin.h>
#include <string>

#define IDC_CLOSE_BUTTON 1001
#define IDC_MENU_BUTTON 1002
#define IDC_RESIZE_BUTTON 1003

class IWTitleBarEventListener {
public:
    virtual void OnResizeStart() = 0;
    virtual void OnIconifyButtonClicked() = 0;
    virtual void OnMenuButtonClicked() = 0;
};

class IWTitleBarBtn;

class IWTitleBar : public CWnd
{
    DECLARE_DYNAMIC(IWTitleBar)
    DECLARE_MESSAGE_MAP()

public:
    IWTitleBar(COLORREF backgroundColor, int fontSize, IWTitleBarEventListener* listener);
    BOOL CreateTopBar(CWnd* pParentWnd, const CRect& rect, UINT nID);
    void SetMenuState(bool isOpen);

    virtual ~IWTitleBar() {}

protected:
    virtual void PositionButtons(const CRect& rect) {};
    virtual void DrawTitle(CDC* pdc, CRect rect, CString title, bool isBeingDragged) {};

    IWTitleBarBtn* menuButton;
    IWTitleBarBtn* iconifyButton;
    IWTitleBarBtn* resizeButton;

    CFont mainFont;

private:
    COLORREF backgroundColor;
    IWTitleBarEventListener* eventListener;
    bool isBeingDragged = false;

    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnIconifyButtonClicked();
    afx_msg void OnMenuButtonClicked();
    afx_msg void OnResizeButtonPressed();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);

    // Window dragging
    CPoint lastPoint;
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void StartDragging(CPoint point);
    afx_msg void StopDragging();
    afx_msg void HandleMouseMove(CPoint point);
};
