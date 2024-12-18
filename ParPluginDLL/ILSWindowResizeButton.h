#pragma once
#include "ILSWindowTitleBarButton.h"

class ILSWindowResizeButton :
    public ILSWindowTitleBarButton
{
    void DrawSymbol(CDC* pDC, CRect rect) override;

    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);


    DECLARE_MESSAGE_MAP()
};

