#pragma once
#include "IWTitleBarBtn.h"

class IWX11ResizeBtn : public IWTitleBarBtn
{
    void DrawSymbol(CDC* pDC, CRect rect) override;

    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);


    DECLARE_MESSAGE_MAP()
};
