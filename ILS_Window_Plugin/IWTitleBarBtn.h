#pragma once
#include <afxwin.h>

class IWTitleBarBtn : public CButton {

DECLARE_MESSAGE_MAP()


public:
    IWTitleBarBtn(COLORREF backgroundColor);
    void SetButtonID(int id);

    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
    virtual void DrawSymbol(CDC* pDC, CRect rect) = 0;

private:
    BOOL mouseOver = FALSE;
    int buttonID = -1;
    COLORREF backgroundColor;
    COLORREF backgroundColorHover;

    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnMouseLeave();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

