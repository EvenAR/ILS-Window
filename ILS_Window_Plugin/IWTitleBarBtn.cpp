#include "pch.h"
#include "IWTitleBarBtn.h"
#include "RenderUtils.h"

BEGIN_MESSAGE_MAP(IWTitleBarBtn, CButton)
    ON_WM_ERASEBKGND()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSELEAVE()
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


IWTitleBarBtn::IWTitleBarBtn(COLORREF backgroundColor)
{
    this->backgroundColor = backgroundColor;
    this->backgroundColorHover = backgroundColor; // TODO
    buttonID = -1; // Set later by the parent if needed
}

BOOL IWTitleBarBtn::OnEraseBkgnd(CDC* pDC) {
    // Do nothing to keep the background transparent
    return TRUE;
}

void IWTitleBarBtn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

    // Get the button's rectangle
    CRect rect = lpDrawItemStruct->rcItem;

    UINT state = lpDrawItemStruct->itemState;

    bool isFocused = (state & ODS_SELECTED) || (state & ODS_HOTLIGHT);

    pDC->FillSolidRect(&rect, this->backgroundColor);  // White background

    // Call DrawSymbol to draw the custom symbol inside
    DrawSymbol(pDC, rect, isFocused);
}

void IWTitleBarBtn::SetButtonID(int id)
{
    buttonID = id; 
}

void IWTitleBarBtn::OnMouseMove(UINT nFlags, CPoint point) {
    CRect rect;
    GetClientRect(&rect);
    if (!mouseOver && rect.PtInRect(point)) {
        mouseOver = TRUE;
        Invalidate(); 
        
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = GetSafeHwnd();
        _TrackMouseEvent(&tme);
    }

    CButton::OnMouseMove(nFlags, point);  
}

void IWTitleBarBtn::OnMouseLeave() {
    if (mouseOver) {
        mouseOver = FALSE;
        Invalidate();
    }
    CButton::OnMouseLeave();
}

void IWTitleBarBtn::OnLButtonDown(UINT nFlags, CPoint point)
{
    CWnd* parent = GetParent();
    if (buttonID != -1 && parent) {
        parent->SendMessage(WM_COMMAND, this->buttonID);
    }
    else {
        CButton::OnLButtonDown(nFlags, point);
    }
}
