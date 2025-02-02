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

    // If the button is hovered, change the background color
    if (mouseOver) {
        pDC->FillSolidRect(&rect, this->backgroundColorHover);  // Light blue background on hover
    }
    else {
        pDC->FillSolidRect(&rect, this->backgroundColor);  // White background
    }

    // Draw border (optional: use a different color on hover)
    CPen pen(PS_SOLID, 1, RGB(0, 0, 0)); // Black pen for the border
    CPen* oldPen = pDC->SelectObject(&pen);
    pDC->SelectStockObject(HOLLOW_BRUSH); // No fill for the rectangle
    pDC->Rectangle(&rect);
    pDC->SelectObject(oldPen);

    // Call DrawSymbol to draw the custom symbol inside
    DrawSymbol(pDC, rect);
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
    mouseOver = TRUE;
    Invalidate();

    CWnd* parent = GetParent();
    if (buttonID != -1 && parent) {
        parent->SendMessage(WM_COMMAND, this->buttonID);
    }
    else {
        CButton::OnLButtonDown(nFlags, point);
    }
}
