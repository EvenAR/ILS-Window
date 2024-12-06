#include "pch.h"
#include "ParWindow.h"

#include <afxwin.h>
#include <gdiplus.h>
using namespace Gdiplus;

BEGIN_MESSAGE_MAP(ParWindow, CWnd)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_LBUTTONDOWN()   // Add the message map entry for WM_LBUTTONDOWN
    ON_WM_LBUTTONUP()     // Add the message map entry for WM_LBUTTONUP
    ON_WM_CTLCOLOR() // Handle custom control colors
    ON_WM_NCCALCSIZE()
    ON_WM_NCPAINT()
    ON_MESSAGE(WM_UPDATE_DATA, &ParWindow::OnUpdateData)
END_MESSAGE_MAP()

BOOL ParWindow::CreateCanvas(CWnd* pParentWnd, const RECT& rect, UINT nID)
{
    CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW);
    return Create(className, _T("Canvas"), WS_CHILD | WS_VISIBLE, rect, pParentWnd, nID);
}

int ParWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Create the custom top bar (move this to WindowTitleBar class)
    CRect barRect(0, 0, lpCreateStruct->cx, 30); // Adjust height as needed
    if (!m_CustomTopBar.CreateTopBar(this, barRect, IDC_TOPBAR))
    {
        AfxMessageBox(_T("Failed to create top bar"));
        return -1;  // Handle error appropriately
    }

    m_CustomTopBar.SetFont(GetFont());
    

    return 0;
}

void ParWindow::OnPaint()
{
    // Clear and repaint the entire window
    InvalidateRect(NULL, TRUE); // Marks the entire client area for repaint, TRUE clears it

    CPaintDC dc(this); // Device context for painting
    CRect rect;
    GetClientRect(&rect); // Get the client area dimensions

    // Example drawing: fill with a gradient
    TRIVERTEX vertex[2];
    GRADIENT_RECT gRect;

    vertex[0].x = rect.left;
    vertex[0].y = rect.top;
    vertex[0].Red = 0x0000;
    vertex[0].Green = 0x8000;
    vertex[0].Blue = 0xFFFF;
    vertex[0].Alpha = 0x0000;

    vertex[1].x = rect.right;
    vertex[1].y = rect.bottom;
    vertex[1].Red = 0xFFFF;
    vertex[1].Green = 0x0000;
    vertex[1].Blue = 0x0000;
    vertex[1].Alpha = 0x0000;

    gRect.UpperLeft = 0;
    gRect.LowerRight = 1;

    dc.GradientFill(vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_H);
}


void ParWindow::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    // Recalculate gradient or redraw on resize
    if (nType != SIZE_MINIMIZED)  // Ignore if window is minimized
    {
        Invalidate(); // Mark the entire client area for repaint
    }

    if (m_CustomTopBar.GetSafeHwnd())
    {
        CRect barRect(0, 0, cx, 30); // Adjust height as needed
        m_CustomTopBar.MoveWindow(barRect);
    }
}

BOOL ParWindow::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CWnd::PreCreateWindow(cs))
        return FALSE;

    // Set initial window styles similar to CreateWindowEx
    cs.dwExStyle |= WS_EX_TOPMOST; // Topmost window
    cs.style = WS_POPUP ; // Custom styles
    cs.cx = 600; // Initial width
    cs.cy = 900; // Initial height
    cs.x = CW_USEDEFAULT; // Default X position
    cs.y = CW_USEDEFAULT; // Default Y position

    return TRUE;
}

LRESULT ParWindow::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
    // TODO: receive data

    // Trigger a repaint
    Invalidate();

    return 0;
}